// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   core/energy_methods/EnvSmoothEnergy.cc
/// @brief  Statistically derived rotamer pair potential class implementation
/// @author Phil Bradley
/// @author Andrew Leaver-Fay


// Unit headers
#include <core/energy_methods/EnvSmoothEnergy.hh>
#include <core/energy_methods/EnvSmoothEnergyCreator.hh>

// Package headers
#include <basic/database/open.hh>
#include <core/chemical/AA.hh>
#include <core/chemical/VariantType.hh>
#include <core/conformation/Atom.hh>
#include <core/id/AtomID.hh>
#include <core/scoring/Energies.hh>
#include <core/scoring/methods/EnergyMethodOptions.hh>
#include <core/scoring/TwelveANeighborGraph.hh>
#include <core/scoring/ContextGraphTypes.hh>

// symmetry
#include <core/pose/symmetry/util.hh>
#include <core/conformation/symmetry/SymmetricConformation.hh>
#include <core/conformation/symmetry/SymmetryInfo.hh>

// Project headers
#include <core/pose/Pose.hh>
#include <core/conformation/Residue.hh>

// Utility headers
#include <utility/io/izstream.hh>
#include <utility/vector1.hh>
#include <utility/vector0.hh>


namespace core {
namespace energy_methods {



/// @details This must return a fresh instance of the EnvSmoothEnergy class,
/// never an instance already in use
core::scoring::methods::EnergyMethodOP
EnvSmoothEnergyCreator::create_energy_method(
	core::scoring::methods::EnergyMethodOptions const & options
) const {
	return utility::pointer::make_shared< EnvSmoothEnergy >( options );
}

core::scoring::ScoreTypes
EnvSmoothEnergyCreator::score_types_for_method() const {
	using namespace core::scoring;
	ScoreTypes sts;
	sts.push_back( envsmooth );
	return sts;
}

Distance const start_sig = 9.8;
Distance const end_sig   = 10.2;

DistanceSquared const start_sig2 = start_sig*start_sig;
DistanceSquared const end_sig2   = end_sig*end_sig;

/// c-tor
EnvSmoothEnergy::EnvSmoothEnergy( core::scoring::methods::EnergyMethodOptions const & options ) :
	parent( utility::pointer::make_shared< EnvSmoothEnergyCreator >() )
{
	initialize( options );
}

EnvSmoothEnergy::EnvSmoothEnergy( EnvSmoothEnergy const & src ) :
	parent( src ),
	envdata_( src.envdata_ )
{}


/// clone
core::scoring::methods::EnergyMethodOP
EnvSmoothEnergy::clone() const
{
	return utility::pointer::make_shared< EnvSmoothEnergy >( *this );
}


/// initialize with envdata from database
void
EnvSmoothEnergy::initialize( core::scoring::methods::EnergyMethodOptions const & options )
{
	// envdata is provided for each aa in 40 bins, corresponding to number of neighbors in 12A neighbor graph
	core::Size const num_bins( 40 );

	envdata_.resize( chemical::num_canonical_aas );

	// check option for alternative envsmooth database file
	utility::io::izstream stream;
	if ( options.envsmooth_zero_negatives() ) {
		basic::database::open( stream, "scoring/score_functions/envsmooth/envdata_zero_negatives.txt" );
	} else {
		basic::database::open( stream, "scoring/score_functions/envsmooth/envdata.txt" );
	}

	core::Real bin_value;
	std::string line;

	// fill envdata_ with values from file
	for ( core::Size ii = 0; ii < chemical::num_canonical_aas; ++ii ) {
		debug_assert( stream );
		getline( stream, line );
		std::istringstream l( line );

		for ( core::Size jj = 0; jj < num_bins; ++jj ) {
			debug_assert( l );
			l >> bin_value;
			envdata_[ ii ].push_back( bin_value );
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// scoring
/////////////////////////////////////////////////////////////////////////////

inline Real sqr ( Real x ) {
	return x*x;
}


/// @details stores dScore/dNumNeighbors so that when neighbor atoms on adjacent
/// residues move, their influence on the score of the surrounding residues is
/// rapidly computed.
void
EnvSmoothEnergy::setup_for_derivatives(
	pose::Pose & pose,
	core::scoring::ScoreFunction const &
) const
{
	pose.update_residue_neighbors();
	Size nres( pose.size() );

	core::conformation::symmetry::SymmetryInfoCOP symm_info;
	if ( core::pose::symmetry::is_symmetric(pose) ) {
		auto & SymmConf (
			dynamic_cast<core::conformation::symmetry::SymmetricConformation &> ( pose.conformation()) );
		symm_info = SymmConf.Symmetry_Info();
	}

	residue_N_.clear();
	residue_E_.clear();
	residue_dEdN_.clear();

	// iterate over all the residues in the protein and count their neighbours
	// and save values of E, N, and dEdN
	for ( Size i = 1; i <= nres; ++i ) {
		if ( symm_info && !symm_info->bb_is_independent( i ) ) {
			residue_E_.push_back(0);
			residue_N_.push_back(0);
			residue_dEdN_.push_back(0);
			continue;
		}

		// get the appropriate residue from the pose.
		conformation::Residue const & rsd( pose.residue(i) );
		// currently this is only for protein residues
		if ( !rsd.is_protein() || rsd.aa() == chemical::aa_unk ) {
			residue_E_.push_back(0);
			residue_N_.push_back(0);
			residue_dEdN_.push_back(0);
			continue; //return;
		}

		Size const atomindex_i = rsd.atom_index( representative_atom_name( rsd.aa() ));

		core::conformation::Atom const & atom_i = rsd.atom(atomindex_i);

		const core::scoring::Energies & energies( pose.energies() );
		const core::scoring::TwelveANeighborGraph & graph ( energies.twelveA_neighbor_graph() );

		Real countN    =  0.0;

		// iterate across neighbors within 12 angstroms
		for ( utility::graph::Graph::EdgeListConstIter
				ir  = graph.get_node(i)->const_edge_list_begin(),
				ire = graph.get_node(i)->const_edge_list_end();
				ir != ire; ++ir ) {
			Size const j( (*ir)->get_other_ind( i ) );
			conformation::Residue const & rsd_j( pose.residue(j) );
			Size atomindex_j( rsd_j.type().nbr_atom() );

			core::conformation::Atom const & atom_j = rsd_j.atom(atomindex_j);

			Real sqdist = atom_i.xyz().distance_squared(atom_j.xyz());
			countN += sigmoidish_neighbor( sqdist );
		}

		Real score = 0;
		Real dscoredN = 0;

		calc_energy( countN, rsd.aa(), score, dscoredN );

		residue_N_.push_back( countN );
		residue_E_.push_back( score );
		residue_dEdN_.push_back( dscoredN );

		//std::cout << "ENV:  " << i << "  " << score << std::endl;
	}

	// symmetrize
	if ( symm_info ) {
		for ( Size i = 1; i <= nres; ++i ) {
			if ( !symm_info->bb_is_independent( i ) ) {
				Size master_i = symm_info->bb_follows( i );
				residue_N_[i] = residue_N_[master_i];
				residue_E_[i] = residue_E_[master_i];
				residue_dEdN_[i] = residue_dEdN_[master_i];
			}
		}
	}
}

void
EnvSmoothEnergy::setup_for_scoring(
	pose::Pose & pose,
	core::scoring::ScoreFunction const &
) const
{
	pose.update_residue_neighbors();
}

/// @details counts the number of nbr atoms within a given radius of the for the input
/// residue.  Because the representative atom on the input residue may be in a different
/// location than the representative atom on the same residue when scoring_begin() is called,
/// these neighbor counts cannot be reused; therefore, scoring_begin does not keep
/// neighbor counts.
void
EnvSmoothEnergy::residue_energy(
	conformation::Residue const & rsd,
	pose::Pose const & pose,
	core::scoring::EnergyMap & emap
) const
{
	// ignore scoring residues which have been marked as "REPLONLY" residues (only the repulsive energy will be calculated)
	if ( rsd.has_variant_type( core::chemical::REPLONLY ) ) {
		return;
	}

	// currently this is only for protein residues
	if ( ! rsd.is_protein() ) return;
	if ( rsd.aa() == chemical::aa_unk ) return;

	core::scoring::TwelveANeighborGraph const & graph ( pose.energies().twelveA_neighbor_graph() );
	Size const atomindex_i = rsd.atom_index( representative_atom_name( rsd.aa() ));

	core::conformation::Atom const & atom_i = rsd.atom(atomindex_i);

	Real countN    =  0.0;
	// iterate across neighbors within 12 angstroms
	for ( utility::graph::Graph::EdgeListConstIter
			ir  = graph.get_node( rsd.seqpos() )->const_edge_list_begin(),
			ire = graph.get_node( rsd.seqpos() )->const_edge_list_end();
			ir != ire; ++ir ) {
		Size const j( (*ir)->get_other_ind( rsd.seqpos() ) );
		conformation::Residue const & rsd_j( pose.residue(j) );

		// if virtual residue, don't score
		if ( rsd_j.aa() == core::chemical::aa_vrt ) continue;

		Size atomindex_j( rsd_j.nbr_atom() );

		core::conformation::Atom const & atom_j = rsd_j.atom(atomindex_j);

		Real sqdist = atom_i.xyz().distance_squared( atom_j.xyz() );
		countN += sigmoidish_neighbor( sqdist );
	}

	Real score = 0, dscoredN = 0;

	calc_energy( countN, rsd.aa(), score, dscoredN );

	emap[ core::scoring::envsmooth ] += score;
}


/// @details Special cases handled for when an atom is both the representative
/// atom for an amino acid, and its nbr_atom.
void
EnvSmoothEnergy::eval_atom_derivative(
	id::AtomID const & atom_id,
	pose::Pose const & pose,
	kinematics::DomainMap const & ,
	core::scoring::ScoreFunction const &,
	core::scoring::EnergyMap const & weights,
	Vector & F1,
	Vector & F2
) const
{
	// ignore scoring residues which have been marked as "REPLONLY" residues (only the repulsive energy will be calculated)
	if ( pose.residue( atom_id.rsd() ).has_variant_type( core::chemical::REPLONLY ) ) {
		return;
	}

	conformation::Residue const & rsd = pose.residue( atom_id.rsd() );

	if ( ! rsd.is_protein() ) return;
	if ( rsd.aa() == chemical::aa_unk ) return;

	Size const i = rsd.seqpos();
	Size const i_nbr_atom = rsd.type().nbr_atom();
	Size const i_rep_atom = rsd.atom_index( representative_atom_name( rsd.aa() ));

	// forces act only on the nbr atom (CB or CA) or the representative atom
	if ( i_nbr_atom != (Size) atom_id.atomno() && i_rep_atom != (Size) atom_id.atomno() ) return;

	core::conformation::Atom const & atom_i = rsd.atom( atom_id.atomno() );

	core::scoring::TwelveANeighborGraph const & graph ( pose.energies().twelveA_neighbor_graph() );

	// its possible both of these are true
	bool const input_atom_is_nbr( i_nbr_atom == Size (atom_id.atomno()) );
	bool const input_atom_is_rep( i_rep_atom == Size ( atom_id.atomno() ));

	Vector f1(0.0), f2(0.0);

	for ( utility::graph::Graph::EdgeListConstIter
			ir  = graph.get_node(i)->const_edge_list_begin(),
			ire = graph.get_node(i)->const_edge_list_end();
			ir != ire; ++ir ) {
		Size const j( (*ir)->get_other_ind( i ) );
		conformation::Residue const & rsd_j( pose.residue(j) );

		// if virtual residue, don't score
		if ( rsd_j.aa() == core::chemical::aa_vrt ) continue;

		if ( input_atom_is_nbr && input_atom_is_rep && (rsd_j.is_protein() && rsd_j.aa()<=core::chemical::num_canonical_aas) ) {
			Size const resj_rep_atom = rsd_j.atom_index( representative_atom_name( rsd_j.aa() ));
			Size const resj_nbr_atom = rsd_j.nbr_atom();
			if ( resj_rep_atom == resj_nbr_atom ) {
				/// two birds, one stone
				increment_f1_f2_for_atom_pair(
					atom_i, rsd_j.atom( resj_rep_atom ),
					weights[ core::scoring::envsmooth ] * ( residue_dEdN_[ j ] + residue_dEdN_[ i ] ),
					F1, F2 );
			} else {
				increment_f1_f2_for_atom_pair(
					atom_i, rsd_j.atom( resj_rep_atom ),
					weights[ core::scoring::envsmooth ] * ( residue_dEdN_[ j ] ),
					F1, F2 );

				increment_f1_f2_for_atom_pair(
					atom_i, rsd_j.atom( resj_nbr_atom ),
					weights[ core::scoring::envsmooth ] * ( residue_dEdN_[ i ] ),
					F1, F2 );
			}
		} else if ( input_atom_is_nbr && (rsd_j.is_protein() && rsd_j.aa()<=core::chemical::num_canonical_aas) ) {
			Size const resj_rep_atom = rsd_j.atom_index( representative_atom_name( rsd_j.aa() ));

			increment_f1_f2_for_atom_pair(
				atom_i, rsd_j.atom( resj_rep_atom ),
				weights[ core::scoring::envsmooth ] * ( residue_dEdN_[ j ] ),
				F1, F2 );

		} else {
			Size const resj_nbr_atom = rsd_j.nbr_atom();
			increment_f1_f2_for_atom_pair(
				atom_i, rsd_j.atom( resj_nbr_atom ),
				weights[ core::scoring::envsmooth ] * ( residue_dEdN_[ i ] ),
				F1, F2 );

		}
	}
}

/// @details returns const & to static data members to avoid expense
/// of string allocation and destruction.  Do not call this function
/// on non-canonical aas
std::string const &
EnvSmoothEnergy::representative_atom_name( chemical::AA const aa ) const
{
	//debug_assert( aa >= 1 && aa <= chemical::num_canonical_aas );

	static std::string const cbeta_string(  "CB"  );
	static std::string const sgamma_string( "SG"  );
	static std::string const cgamma_string( "CG"  );
	static std::string const cdelta_string( "CD"  );
	static std::string const czeta_string(  "CZ"  );
	static std::string const calpha_string( "CA"  );
	static std::string const ceps_1_string( "CE1" );
	static std::string const cdel_1_string( "CD1" );
	static std::string const ceps_2_string( "CE2" );
	static std::string const sdelta_string( "SD"  );

	switch ( aa ) {
	case ( chemical::aa_ala ) : return cbeta_string;
	case ( chemical::aa_cys ) : return sgamma_string;
	case ( chemical::aa_asp ) : return cgamma_string;
	case ( chemical::aa_glu ) : return cdelta_string;
	case ( chemical::aa_phe ) : return czeta_string;
	case ( chemical::aa_gly ) : return calpha_string;
	case ( chemical::aa_his ) : return ceps_1_string;
	case ( chemical::aa_ile ) : return cdel_1_string;
	case ( chemical::aa_lys ) : return cdelta_string;
	case ( chemical::aa_leu ) : return cgamma_string;
	case ( chemical::aa_met ) : return sdelta_string;
	case ( chemical::aa_asn ) : return cgamma_string;
	case ( chemical::aa_pro ) : return cgamma_string;
	case ( chemical::aa_gln ) : return cdelta_string;
	case ( chemical::aa_arg ) : return czeta_string;
	case ( chemical::aa_ser ) : return cbeta_string;
	case ( chemical::aa_thr ) : return cbeta_string;
	case ( chemical::aa_val ) : return cbeta_string;
	case ( chemical::aa_trp ) : return ceps_2_string;
	case ( chemical::aa_tyr ) : return czeta_string;
	default :
		utility_exit_with_message( "ERROR: Failed to find amino acid " + chemical::name_from_aa( aa ) + " in EnvSmooth::representative_atom_name" );
		break;
	}

	// unreachable
	return calpha_string;
}

/// @brief EnvSmoothEnergy distance cutoff
Distance
EnvSmoothEnergy::atomic_interaction_cutoff() const
{
	return 0.0;
}

/// @brief EnvSmoothEnergy
void
EnvSmoothEnergy::indicate_required_context_graphs( utility::vector1< bool > & context_graphs_required ) const
{
	context_graphs_required[ core::scoring::twelve_A_neighbor_graph ] = true;
}

void
EnvSmoothEnergy::calc_energy(
	Real const neighbor_count,
	chemical::AA const aa,
	Real & score,
	Real & dscore_dneighbor_count
) const
{
	auto low_bin = static_cast< Size > ( floor(neighbor_count));
	auto high_bin = static_cast< Size > ( ceil(neighbor_count));
	Real inter = neighbor_count - low_bin;

	auto const aa_as_int = static_cast< int > (aa);

	if ( high_bin < 40 ) {
		score = envdata_[ aa_as_int - 1 ][ low_bin ]  * (1.0-inter) +
			envdata_[ aa_as_int - 1 ][ high_bin ] * (inter);
		dscore_dneighbor_count = envdata_[ aa_as_int - 1 ][ high_bin ] -
			envdata_[ aa_as_int - 1 ][ low_bin ];
	} else {
		score = envdata_[ aa_as_int - 1 ][ 39 ];
		dscore_dneighbor_count = 0;
	}
	score *= 2.019; // this factor is from rosetta++ and fuck knows where it came from originally :-)
	dscore_dneighbor_count *= 2.019;
}

Real
EnvSmoothEnergy::sigmoidish_neighbor( DistanceSquared const sqdist ) const
{
	if ( sqdist > end_sig2 ) {
		return 0.0;
	} else if ( sqdist < start_sig2 ) {
		return 1.0;
	} else {
		Real dist = sqrt( sqdist );
		return sqr(1.0  - sqr( (dist - start_sig) / (end_sig - start_sig) ) );
	}
}


void
EnvSmoothEnergy::increment_f1_f2_for_atom_pair(
	conformation::Atom const & atom1,
	conformation::Atom const & atom2,
	Real weighted_dScore_dN,
	Vector & F1,
	Vector & F2
) const
{
	DistanceSquared dist2 = atom1.xyz().distance_squared(atom2.xyz());
	Distance dist( 0.0 ); // only used if start_sig2 <= dist2 <= end_sig2

	Real dNdd = 0;
	if ( dist2 > end_sig2 ) {
		dNdd = 0;
	} else if ( dist2 < start_sig2 ) {
		dNdd = 0.0;
	} else {
		dist = sqrt( dist2 );
		Real x = (dist - start_sig)/ (end_sig - start_sig);
		dNdd = 4*x*(-1 + x*x) / (end_sig - start_sig);
	}

	Real dscoredd = ( weighted_dScore_dN ) * dNdd;
	if ( dscoredd != 0 ) {

		Vector const f1( cross( atom1.xyz(), atom2.xyz() ));
		Vector const f2( atom1.xyz() - atom2.xyz() );

		dscoredd /= dist;
		F1 += dscoredd * f1;
		F2 += dscoredd * f2;
	}
}

core::Size
EnvSmoothEnergy::version() const
{
	return 1; // Initial versioning
}
}
}

