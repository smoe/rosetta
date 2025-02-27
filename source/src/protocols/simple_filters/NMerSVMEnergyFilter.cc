// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file protocols/simple_filters/NMerSVMEnergyFilter.cc
/// @brief
/// @author Indigo King (indigo.c.king@gmail.com)


//Unit Headers
#include <protocols/simple_filters/NMerSVMEnergyFilter.hh>
#include <protocols/simple_filters/NMerSVMEnergyFilterCreator.hh>

//Project Headers
#include <basic/Tracer.hh>
#include <core/scoring/nmer/NMerSVMEnergy.hh>
#include <core/pose/Pose.hh>
#include <core/pose/PDBInfo.hh>
#include <utility/tag/Tag.hh>
#include <basic/datacache/DataMap.fwd.hh>
#include <utility/excn/Exceptions.hh>
#include <core/pose/selection.hh>
#include <protocols/jd2/util.hh>
#include <utility/io/ozstream.hh>
#include <utility/string_util.hh>
// XSD XRW Includes
#include <utility/tag/XMLSchemaGeneration.hh>
#include <protocols/filters/filter_schemas.hh>

namespace protocols {
namespace simple_filters {

using namespace core;
using namespace utility;
using namespace core::scoring;

static basic::Tracer TR( "protocols.simple_filters.NMerSVMEnergyFilter" );



//default ctor
NMerSVMEnergyFilter::NMerSVMEnergyFilter() :
	protocols::filters::Filter( "NMerSVMEnergy" )
{}

//full ctor, default ctor defined in header file
//TODO: allow setting energy_method_ funxns w/ input params (like in parse ctor)
NMerSVMEnergyFilter::NMerSVMEnergyFilter(
	core::Real const score_type_threshold,
	std::string const string_resnums
) :
	protocols::filters::Filter( "NMerSVMEnergy" )
{
	score_type_threshold_ = score_type_threshold;
	string_resnums_ = string_resnums;
}

NMerSVMEnergyFilter::~NMerSVMEnergyFilter() = default;

void
NMerSVMEnergyFilter::parse_my_tag(
	utility::tag::TagCOP tag,
	basic::datacache::DataMap & /*data*/
) {
	if ( ! tag->hasOption( "threshold" ) ) throw CREATE_EXCEPTION(utility::excn::RosettaScriptsOptionError, "Must specify 'threshold' for NMerSVMEnergyFilter.");
	score_type_threshold_ = tag->getOption< core::Real >( "threshold" );

	if ( tag->hasOption( "svm_fname" ) ) energy_method_.read_nmer_svm( tag->getOption< std::string >( "svm_fname" ) );
	if ( tag->hasOption( "svm_list_fname" ) ) energy_method_.read_nmer_svm_list( tag->getOption< std::string >( "svm_list_fname" ) );
	//default blank string --> empty res_set_vec --> incl all residues
	string_resnums_ = tag->getOption< std::string >( "resnums", "" );// these are kept in memory until the pose is available (at apply time)
	if ( tag->hasOption( "nmer_length" ) ) energy_method_.nmer_length( tag->getOption< core::Size >( "nmer_length", 9 ) );
	if ( tag->hasOption( "nmer_svm_scorecut" ) ) energy_method_.nmer_svm_scorecut( tag->getOption< core::Real >( "nmer_svm_scorecut", 0.0 ) );
	if ( tag->hasOption( "gate_svm_scores" ) ) energy_method_.gate_svm_scores( tag->getOption< bool >( "gate_svm_scores", false ) );
	dump_table_ = tag->getOption< bool >( "dump_table", false );
	rank_as_score_ = tag->getOption< bool >( "use_rank_as_score", false );
	count_eps_ = tag->getOption< bool >( "count_eps", false );
	ep_cutoff_ = tag->getOption< core::Real >( "ep_cutoff", core::Real( 0.425 ) ); //is 500nM binding
}

bool
NMerSVMEnergyFilter::apply( core::pose::Pose const & pose ) const {
	core::Real const score( compute( pose ) );
	TR << "NMerSVM score is " << score << ". ";
	if ( score <= score_type_threshold_ ) {
		TR<<"passing." << std::endl;
		return true;
	} else {
		TR<<"failing."<<std::endl;
		return false;
	}
}

void
NMerSVMEnergyFilter::report( std::ostream & out, core::pose::Pose const & pose ) const {
	out<<"NMerSVM score of " << compute( pose )<<'\n';
}

core::Real
NMerSVMEnergyFilter::report_sm( core::pose::Pose const & pose ) const {
	return( compute( pose ) );
}

struct nmer_svm_res_data{
	core::Size seqpos;
	std::string pdb_seqpos;
	Real energy;
	vector1< Real > svm_energies;
	vector1< Real > svm_ranks;
	Real svm_rank_avg;
	std::string nmer_seq;
};

/*
void
NMerSVMEnergyFilter::compute_residue(
core::pose::Pose const & pose,
core::Size const seqpos,
Real & rsd_energy,
vector1< Real > & rsd_svm_energies
) const {
assert( seqpos <= pose.size() );
energy_method_.get_residue_energy_by_svm( pose, seqpos, rsd_energy, rsd_svm_energies );
}
*/
//TODO: option for printing out resfile?

void
print_nmer_svm_energy_data(
	vector1< nmer_svm_res_data > nmer_svm_pose_data
) {
	std::string fname( "nmer_svm_table" );
	if ( protocols::jd2::jd2_used() ) fname += ( "." + protocols::jd2::current_output_name() );
	fname += ".tab";
	// utility::io::ozstream outtable( fname, std::ios::out | std::ios::app ); // Append if logfile already exists.
	utility::io::ozstream outtable( fname, std::ios::out ); // Overwrite if logfile already exists.

	for ( core::Size idat = 1; idat <= nmer_svm_pose_data.size(); ++idat ) {
		outtable << nmer_svm_pose_data[ idat ].pdb_seqpos << "\t" << nmer_svm_pose_data[ idat ].nmer_seq << "\t";
		for ( core::Size isvm = 1; isvm <= nmer_svm_pose_data[ idat ].svm_energies.size(); ++isvm ) {
			outtable << isvm << ":" << nmer_svm_pose_data[ idat ].svm_energies[ isvm ] << ":" << nmer_svm_pose_data[ idat ].svm_ranks[ isvm ] << "\t";
		}
		outtable << "AVG:" << nmer_svm_pose_data[ idat ].energy << ":" << nmer_svm_pose_data[ idat ].svm_rank_avg;
		outtable << std::endl;
	}
	outtable.close();
}

core::Real
NMerSVMEnergyFilter::compute(
	core::pose::Pose const & pose
) const {

	utility::vector1< core::Size > const res_set_vec( core::pose::get_resnum_list_ordered( string_resnums_, pose ) );
	// sum over all res pos
	core::Real score( 0. );
	core::Real rank( 0. );
	core::Size n_eps( 0 );
	vector1< nmer_svm_res_data > nmer_svm_pose_data;
	for ( core::Size seqpos = 1; seqpos <= pose.size(); ++seqpos ) {
		bool incl_rsd = true; //calc info for this sequence position?
		//if user defined a set of sequence positions, then see if seqpos is in that set
		if ( res_set_vec.size() > 0 ) {
			incl_rsd = false;
			for ( core::Size i_res_vec = 1; i_res_vec <= res_set_vec.size(); ++i_res_vec ) {
				if ( res_set_vec[ i_res_vec ] == seqpos ) {
					incl_rsd = true;
					break;
				}
			}
		}
		if ( !incl_rsd ) continue;

		Real rsd_energy( 0. );
		Real rsd_rank( 0. );
		vector1< Real > rsd_svm_energies( energy_method_.n_svms(), Real( 0. ) );
		vector1< Real > rsd_svm_ranks( energy_method_.n_svms(), Real( 0. ) );
		energy_method_.get_residue_energy_by_svm( pose, seqpos, rsd_energy, rsd_rank, rsd_svm_energies, rsd_svm_ranks );
		std::string pdb_seqpos( utility::to_string( seqpos ) );
		if ( pose.pdb_info().get() != nullptr ) pdb_seqpos = pose.pdb_info()->pose2pdb( seqpos );
		//get this nmer sequence, just ignore if we fall off the end
		std::string nmer_seq( "" );
		if ( seqpos <= pose.size() - energy_method_.nmer_length() + 1 ) {
			nmer_seq = pose.sequence().substr( seqpos - 1, energy_method_.nmer_length() );
		}
		//store nmer_svm energy of this seqpos and each individual svms contribution in a struct
		nmer_svm_res_data rsd_data;
		rsd_data.seqpos = seqpos;
		rsd_data.pdb_seqpos = pdb_seqpos;
		rsd_data.energy = rsd_energy;
		rsd_data.svm_energies = rsd_svm_energies;
		rsd_data.svm_ranks = rsd_svm_ranks;
		rsd_data.svm_rank_avg = rsd_rank;
		rsd_data.nmer_seq = nmer_seq;
		//and push that struct onto this vector
		nmer_svm_pose_data.push_back( rsd_data );
		score += rsd_energy;
		rank += rsd_rank;
		//if we want to know total discreet eps > cutoff, unpack rsd_svm_energies and incr ep counter
		for ( core::Size isvm = 1; isvm <= rsd_svm_energies.size(); isvm++ ) {
			// use rank avg as score for epitope cutoff?
			if ( rank_as_score_ && rsd_svm_ranks[ isvm ] > ep_cutoff_ ) n_eps++;
			// ...or use raw score
			else if ( rsd_svm_energies[ isvm ] > ep_cutoff_ ) n_eps++;
		}
	}
	// dump tab delim data to file?
	if ( dump_table_ ) print_nmer_svm_energy_data( nmer_svm_pose_data );
	// resturn # epitopes, sum of avg rank, or sum of avg svm energy?
	if ( count_eps_ ) return( static_cast< core::Real >( n_eps ) );
	else if ( rank_as_score_ ) return( static_cast< core::Real >( rank  ) );
	else return( score );
}

std::string NMerSVMEnergyFilter::name() const {
	return class_name();
}

std::string NMerSVMEnergyFilter::class_name() {
	return "NMerSVMEnergy";
}

void NMerSVMEnergyFilter::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd )
{
	using namespace utility::tag;
	AttributeList attlist;
	attlist + XMLSchemaAttribute::required_attribute("threshold", xsct_real, "cutoff, fail if score greater than cutoff")
		+ XMLSchemaAttribute("svm_fname", xs_string, "single name of a libsvm .model file")
		+ XMLSchemaAttribute("svm_list_fname", xs_string, "filename to a list of svm .model filenames")
		+ XMLSchemaAttribute::attribute_w_default("resnums", xs_string, "optional subset of residue numbers over which to do calculation", "XRW TO DO")
		+ XMLSchemaAttribute::attribute_w_default("nmer_length", xsct_non_negative_integer, "number of residues in core subsequence motif", "9")
		+ XMLSchemaAttribute::attribute_w_default("nmer_svm_scorecut", xsct_real, "optional lower bound floor to returned scores", "0.0")
		+ XMLSchemaAttribute::attribute_w_default("gate_svm_scores", xsct_rosetta_bool, "option to use lower bound floor for svm scores", "false")
		+ XMLSchemaAttribute::attribute_w_default("dump_table", xsct_rosetta_bool, "dump a table of all nmer v. svm model scores", "false")
		+ XMLSchemaAttribute::attribute_w_default("use_rank_as_score", xsct_rosetta_bool, "report normalized rank fraction instead of raw energy", "false")
		+ XMLSchemaAttribute::attribute_w_default("count_eps", xsct_rosetta_bool, "report number of epitopes w score greater than cutoff", "false")
		+ XMLSchemaAttribute::attribute_w_default("ep_cutoff", xsct_real, "optional epitope score cutiff for boolean count return option", "0.425");

	protocols::filters::xsd_type_definition_w_attributes( xsd, class_name(), "XRW TO DO", attlist );
}

std::string NMerSVMEnergyFilterCreator::keyname() const {
	return NMerSVMEnergyFilter::class_name();
}

protocols::filters::FilterOP
NMerSVMEnergyFilterCreator::create_filter() const {
	return utility::pointer::make_shared< NMerSVMEnergyFilter >();
}

void NMerSVMEnergyFilterCreator::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd ) const
{
	NMerSVMEnergyFilter::provide_xml_schema( xsd );
}


}
}
