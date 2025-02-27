// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file
/// @brief take out the "compute_frag_bias()" from CartesianSampler
/// @author Ray Wang wangyr@uw.edu
//
#ifndef INCLUDED_protocols_hybridization_FragmentBiasAssigner_hh
#define INCLUDED_protocols_hybridization_FragmentBiasAssigner_hh


#include <core/pose/Pose.fwd.hh>

#include <core/scoring/ScoreType.hh>

#include <core/fragment/FragSet.fwd.hh>

// symmetry
#include <core/conformation/symmetry/SymmetryInfo.fwd.hh>

#include <numeric/random/WeightedSampler.fwd.hh>

#include <utility/vector1.hh>

#include <protocols/loops/Loops.fwd.hh>

#include <utility/VirtualBase.hh>

#include <map> // AUTO IWYU For map, allocator
#include <set> // AUTO IWYU For set

namespace protocols {
namespace hybridization {

class FragmentBiasAssigner : public utility::VirtualBase
{
public:
	// constructor
	//FragmentBiasAssigner();
	FragmentBiasAssigner( core::pose::Pose &pose );

	void init( core::pose::Pose &pose );

	// everytime when you call these function the probability is going to be additive to the frag_bias_ container
	void compute_frag_bias( utility::vector1<numeric::random::WeightedSampler> &frag_bias, // output
		core::pose::Pose &pose,
		utility::vector1<core::fragment::FragSetOP> fragments );

	void include_residues( std::set< core::Size > residues_to_include );

	void exclude_residues( std::set< core::Size > residues_to_exclude );

	/////////////////////////////////////////////////////////////
	void uniform();

	////////////////////////////////////
	// ray's protocol to select residues to refine
	void automode( core::pose::Pose & pose,
		core::Real score_cut );

	void automode_scores( core::pose::Pose & pose,
		std::map< core::Size, core::Real > & scores);

	/////////////////////////////////////////////////////////////
	void user( std::set<core::Size> user_pos,
		protocols::loops::LoopsOP loops );

	////////////////////////////////////
	// individual method

	void density_nbr( core::pose::Pose & pose );

	void rama( core::pose::Pose & pose,
		core::Real weight=0.2 );

	void geometry( core::pose::Pose & pose,
		core::Real weight=1.0 );

	void density( core::pose::Pose &pose );

	////////////////////////////////
	void chainbreak( core::pose::Pose &pose );
	void bfactors( core::pose::Pose &pose );
	void fragbias_reporter( core::pose::Pose &pose );
	void cumulate_probability(){ cumulative_=true; }

	void set_rsd_wdw_to_assign_prob( int wdw=0 ){ rsd_wdw_size_=wdw; }
	void set_wdw_to_freeze( int wdw=0 ){ wdw_to_freeze_=wdw; }
	void set_score_threshold( core::Real threshold ){ score_threshold_=threshold; }

private:
	// functions
	void cal_perrsd_score( core::pose::Pose &pose,
		core::scoring::ScoreType const &score_type,
		utility::vector1<core::Real> &perrsd_score,
		core::Real weight );

	// This function calls assign_prob_with_rsd_wdw(rsn) to assign probability to the residue with a window controlled by "rsd_wdw_size_".
	void assign_fragprobs( std::map< core::Size, core::Real > const & perrsd_score,
		core::Real threshold );

	void assign_prob_with_rsd_wdw( int rsn );


	// variables
	core::Size nres_, n_symm_subunit_;
	int wdw_to_freeze_;
	int rsd_wdw_size_; // to assign prob
	core::Real score_threshold_; // to assign prob
	bool cumulative_;
	bool fragProbs_assigned_;
	core::conformation::symmetry::SymmetryInfoCOP symminfo_;

	////////////////////////////////////
	// containers
	// the per-residue based container for all the methods to add values into it
	utility::vector1<core::Real> fragmentProbs_;

	// the per residue container for score terms
	std::map< core::Size, core::Real > perrsd_dens_;
	std::map< core::Size, core::Real > perrsd_nbrdens_;
	std::map< core::Size, core::Real > perrsd_rama_;
	std::map< core::Size, core::Real > perrsd_geometry_;

	//utility::vector1<numeric::random::WeightedSampler> frag_bias_;
}; // class FragmentBiasAssigner

} // hybridization
} // protocol
#endif
