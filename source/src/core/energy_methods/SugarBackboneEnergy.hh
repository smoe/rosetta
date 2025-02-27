// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file    core/energy_methods/SugarBackboneEnergy.hh
/// @brief   Method declarations and simple accessor definitions for SugarBackboneEnergy.
/// @author  Labonte <JWLabonte@jhu.edu>


#ifndef INCLUDED_core_energy_methods_carbohydrates_SugarBackboneEnergy_HH
#define INCLUDED_core_energy_methods_carbohydrates_SugarBackboneEnergy_HH

// Unit header
#include <core/energy_methods/SugarBackboneEnergy.fwd.hh>
#include <core/scoring/carbohydrates/CHIEnergyFunction.fwd.hh>
#include <core/scoring/carbohydrates/OmegaPreferencesFunction.fwd.hh>

// Package headers
#include <core/scoring/ScoreFunction.fwd.hh>
#include <core/scoring/methods/ContextIndependentOneBodyEnergy.hh>

// Project headers
#include <core/id/DOF_ID.fwd.hh>
#include <core/conformation/Residue.fwd.hh>
#include <core/pose/Pose.fwd.hh>


namespace core {
namespace energy_methods {



/// @details  This class makes use of the "CarboHydrate Intrinsic" (CHI) Energy Functions developed by Woods Lab.\n
/// Carbohydrate phi angles are scored based on whether they are at alpha or beta linkages.\n
/// Psi angles are scored based on whether they are at ->2-axial, ->3-equatorial, or ->4-axial OR ->2-equatorial,
/// ->3-axial, or ->4-equatorial linkages OR alpha6 linkages OR beta6 linkages.\n
/// Omega angles (in ->6-linkages) are scored with a different function, based on whether O4 (if present) is axial or
/// equatorial, which is the origin of the so-called gauche effect.
/// All other glycosidic linkages are not scored.
/// @ref      A.K. Nivedha et al. J. Comput. Chem. 2014, 35, 526-39
/// @ref      A.K. Nivedha et al. JCTC 2016, 12, 892-901
class SugarBackboneEnergy : public core::scoring::methods::ContextIndependentOneBodyEnergy {
public:  // Standard Methods //////////////////////////////////////////////////
	/// @brief  Default constructor
	SugarBackboneEnergy();


public:  // General EnergyMethod Methods //////////////////////////////////////
	core::scoring::methods::EnergyMethodOP clone() const override;

	/// @brief   Should this EnergyMethod have score and derivative evaluation evaluated ONLY in the context of a whole
	/// Pose?
	/// @return  false
	bool minimize_in_whole_structure_context( pose::Pose const & ) const override { return false; }

	/// @brief    Indicate in the context-graphs-required list which context-graphs this energy method requires that the
	/// Pose maintains when doing neighbor evaluation.
	/// @details  not implemented for SugarBackboneEnergy
	void indicate_required_context_graphs( utility::vector1< bool > & /*context_graphs_required*/ ) const override {}


public:  // OneBodyEnergy Methods /////////////////////////////////////////////
	/// @brief  Evaluate the one-body carbohydrate backbone energies for a particular residue, in the context of the
	/// given Pose, and increment those energies in the input Emap.
	void residue_energy( conformation::Residue const & rsd, pose::Pose const & pose, core::scoring::EnergyMap & emap ) const override;

	/// @brief   Should the dof_derivative interface be used for this EnergyMethod when calculating derivatives?
	/// @return  true
	bool defines_dof_derivatives( pose::Pose const & /* pose */ ) const override { return true; }

	utility::vector1< id::PartialAtomID >
	atoms_with_dof_derivatives( conformation::Residue const & res, pose::Pose const & pose ) const override;

	/// @brief    Evaluate the DoF derivative for a particular residue.
	core::Real
	eval_residue_dof_derivative(
		conformation::Residue const & rsd,
		core::scoring::ResSingleMinimizationData const & min_data,
		id::DOF_ID const & dof_id,
		id::TorsionID const & torsion_id,
		pose::Pose const & pose,
		core::scoring::ScoreFunction const & sf,
		core::scoring::EnergyMap const & weights
	) const override;


private:  // Private methods //////////////////////////////////////////////////
	core::Size version() const override { return 1; }  // initial versioning

private:  // Private Data /////////////////////////////////////////////////////
	// the "CarboHydrate Intrinsic" (CHI) Energy Function developed by Woods Lab
	scoring::carbohydrates::CHIEnergyFunction const & E_cef_;

	// an energy function for omega preferences
	scoring::carbohydrates::OmegaPreferencesFunction const & E_opf_;
};

}  // namespace energy_methods
}  // namespace core

#endif  // INCLUDED_core_energy_methods_carbohydrates_SugarBackboneEnergy_HH
