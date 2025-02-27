// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   core/energy_methods/SymmetricLigandEnergy.hh
/// @brief  score for implicit ligand interactions from symmetric geometry
/// @author Will Sheffler


#ifndef INCLUDED_core_energy_methods_SymmetricLigandEnergy_hh
#define INCLUDED_core_energy_methods_SymmetricLigandEnergy_hh

// Unit headers
#include <core/energy_methods/SymmetricLigandEnergy.fwd.hh>


// Package headers
#include <core/scoring/methods/ContextIndependentOneBodyEnergy.hh>
#include <core/scoring/ScoreFunction.fwd.hh>

// Project headers
#include <core/pose/Pose.fwd.hh>
#include <core/kinematics/DomainMap.fwd.hh>

#include <core/types.hh>

#include <utility/vector1.hh>


namespace core {
namespace energy_methods {



class SymmetricLigandEnergy : public core::scoring::methods::ContextIndependentOneBodyEnergy {
public:

	typedef core::scoring::methods::ContextIndependentOneBodyEnergy  parent;

	/// @brief ctor
	SymmetricLigandEnergy();

	/// @brief dtor
	~SymmetricLigandEnergy() override;

	/// clone
	core::scoring::methods::EnergyMethodOP
	clone() const override;

	/////////////////////////////////////////////////////////////////////////////
	// methods for ContextIndependentOneBodyEnergies
	/////////////////////////////////////////////////////////////////////////////


	void
	residue_energy(
		conformation::Residue const & rsd,
		pose::Pose const & pose,
		core::scoring::EnergyMap & emap
	) const override;


	void
	eval_atom_derivative(
		id::AtomID const & id,
		pose::Pose const & pose,
		kinematics::DomainMap const & domain_map,
		core::scoring::ScoreFunction const & sfxn,
		core::scoring::EnergyMap const & emap,
		Vector & F1,
		Vector & F2
	) const override;

	/// @brief SymmetricLigandEnergy is context independent; indicates that no
	/// context graphs are required
	void indicate_required_context_graphs( utility::vector1< bool > & ) const override;
	core::Size version() const override;


};

} // scoring
} // core


#endif // INCLUDED_core_scoring_SymmetricLigandEnergy_HH
