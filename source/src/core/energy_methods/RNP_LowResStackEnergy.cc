// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   core/scoring/rna/RNP_LowResStackEnergy.cc
/// @brief  Statistically derived rotamer pair potential class implementation
/// @author Kalli Kappel


// Unit headers
#include <core/energy_methods/RNP_LowResStackEnergy.hh>
#include <core/scoring/rna/RNP_LowResStackData.hh>
#include <core/energy_methods/RNP_LowResStackEnergyCreator.hh>

// Package headers
#include <basic/Tracer.hh>
#include <core/scoring/ScoringManager.hh>

// Project headers
#include <core/pose/Pose.fwd.hh>
#include <core/conformation/Residue.hh>

#include <basic/options/keys/score.OptionKeys.gen.hh>

// Utility headers

#include <numeric/xyzMatrix.hh>
#include <numeric/xyzVector.hh>

//Auto Headers

#include <core/chemical/rna/util.hh> // AUTO IWYU For get_rna_base_centroid, get_rna_base_coordinat...
#include <core/scoring/EnergyMap.hh> // AUTO IWYU For EMapVector


///////////////////////////////////////////////////////////////////////////////////////////////////
//
// This is a statistically derived low-resolution potential for RNA/protein interactions
// For RNA/protein modeling, this is meant to supplement the RNA low-res and protein low-res score
// functions
//
///////////////////////////////////////////////////////////////////////////////////////////////////

static basic::Tracer TR( "core.energy_methods.RNP_LowResStackEnergy" );
using namespace core::chemical;
using namespace core::chemical::rna;
using namespace basic::options;
using namespace basic::options::OptionKeys::score;

namespace core {
namespace energy_methods {

/// @details This must return a fresh instance of the RNP_LowResStackEnergy class,
/// never an instance already in use
core::scoring::methods::EnergyMethodOP
RNP_LowResStackEnergyCreator::create_energy_method(
	core::scoring::methods::EnergyMethodOptions const &
) const {
	return utility::pointer::make_shared< RNP_LowResStackEnergy >();
}

core::scoring::ScoreTypes
RNP_LowResStackEnergyCreator::score_types_for_method() const {
	using namespace core::scoring;
	ScoreTypes sts;
	sts.push_back( rnp_stack_xy );
	return sts;
}


/// c-tor
RNP_LowResStackEnergy::RNP_LowResStackEnergy() :
	parent( utility::pointer::make_shared< RNP_LowResStackEnergyCreator >() ),
	potential_( core::scoring::ScoringManager::get_instance()->get_RNP_LowResStackData() )
{
	//std::cout << "Constructed the RNP stack energy" << std::endl;
}

//clone
core::scoring::methods::EnergyMethodOP
RNP_LowResStackEnergy::clone() const
{
	return utility::pointer::make_shared< RNP_LowResStackEnergy >();
}

/////////////////////////////////////////////////////////////////////////////
// scoring
/////////////////////////////////////////////////////////////////////////////
//void
//RNP_LowResStackEnergy::setup_for_scoring( pose::Pose & pose, core::scoring::ScoreFunction const & scfxn ) const
//{
//}
//
//void
//RNP_LowResStackEnergy::setup_for_derivatives( pose::Pose & pose, core::scoring::ScoreFunction const & ) const
//{
//}

//////////////////////////////////////////////////////////////////////////////////////////
void
RNP_LowResStackEnergy::residue_pair_energy(
	conformation::Residue const & rsd1,
	conformation::Residue const & rsd2,
	pose::Pose const &,
	core::scoring::ScoreFunction const &,
	core::scoring::EnergyMap & emap
) const
{
	if ( rsd1.has_variant_type( REPLONLY ) ) return;
	if ( rsd2.has_variant_type( REPLONLY ) ) return;

	// Only evaluate these score terms between RNA and protein residues
	if ( !(( rsd1.is_RNA() && rsd2.is_protein() ) || ( rsd1.is_protein() && rsd2.is_RNA() )) ) return;

	Vector rna_centroid;
	Vector protein_centroid;
	numeric::xyzMatrix< core::Real > rna_base_coord_sys;

	if ( rsd1.is_RNA() ) {
		rna_centroid = chemical::rna::get_rna_base_centroid( rsd1 );
		rna_base_coord_sys = chemical::rna::get_rna_base_coordinate_system( rsd1, rna_centroid );
		protein_centroid = rsd2.xyz( "CEN" );
	} else {
		rna_centroid = chemical::rna::get_rna_base_centroid( rsd2 );
		rna_base_coord_sys = chemical::rna::get_rna_base_coordinate_system( rsd2, rna_centroid );
		protein_centroid = rsd1.xyz( "CEN" );
	}
	Vector x_rna = rna_base_coord_sys.col_x();
	Vector y_rna = rna_base_coord_sys.col_y();
	Vector z_rna = rna_base_coord_sys.col_z();

	Vector dist_rna_protein = protein_centroid - rna_centroid;
	Real const dist_x = dot_product( dist_rna_protein, x_rna );
	Real const dist_y = dot_product( dist_rna_protein, y_rna );
	Real const dist_z = dot_product( dist_rna_protein, z_rna );

	// Get the stack score
	Real rnp_stack_xy_score( 0.0 );
	if ( std::abs(dist_z) > 3.0 && std::abs(dist_z) < 6.5 ) {
		potential_.evaluate_rnp_stack_xy_score( rsd1, rsd2, dist_x, dist_y, rnp_stack_xy_score );
	}
	emap[ core::scoring::rnp_stack_xy ] += rnp_stack_xy_score;

}


//////////////////////////////////////////////////////////////////////////////////////////
//void
//RNP_LowResStackEnergy::finalize_total_energy(
// pose::Pose & pose,
// core::scoring::ScoreFunction const &,
// core::scoring::EnergyMap &
//) const {
//
//}

//??
/// @brief RNA_PairwiseLowResolutionEnergy distance cutoff
Distance
RNP_LowResStackEnergy::atomic_interaction_cutoff() const
{
	return 0.0; /// Uh, I don't know.
}

core::Size
RNP_LowResStackEnergy::version() const
{
	return 1; // Initial versioning
}

//etable::count_pair::CountPairFunctionCOP
//RNP_LowResStackEnergy::get_intrares_countpair(
// conformation::Residue const &,
// pose::Pose const &,
// core::scoring::ScoreFunction const &
//) const
//{
// utility_exit_with_message( "FA_ElecEnergy does not define intra - residue pair energies; do not call get_intrares_countpair()" );
// return 0;
//}
//
//etable::count_pair::CountPairFunctionCOP
//RNP_LowResStackEnergy::get_count_pair_function(
// Size const res1,
// Size const res2,
// pose::Pose const & pose,
// core::scoring::ScoreFunction const &
//) const
//{
//}
//
//
//etable::count_pair::CountPairFunctionCOP
//RNP_LowResStackEnergy::get_count_pair_function(
// conformation::Residue const & rsd1,
// conformation::Residue const & rsd2
//) const
//{
//}


} //scoring
} //core
