// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   core/pack/task/residue_selector/ClashBasedShellSelectorCreator.hh
/// @brief  The ClashBasedShellSelector identifies all residues that clash with at least one rotamer of a design position
/// @details Since this ResidueSelector is located in a different namespace, it needs a separate ResidueSelectorCreator.
/// @author Noah Ollikainen (nollikai@gmail.com)
/// @author Roland A. Pache, PhD
/// @author Vikram K. Mulligan, PhD (vmullig@uw.edu)

#ifndef INCLUDED_core_pack_task_residue_selector_ClashBasedShellSelectorCreator_HH
#define INCLUDED_core_pack_task_residue_selector_ClashBasedShellSelectorCreator_HH

// Unit headers
#include <core/select/residue_selector/ResidueSelectorCreator.hh>

// Utility headers
#include <utility/tag/XMLSchemaGeneration.fwd.hh>

namespace core {
namespace pack {
namespace task {
namespace residue_selector {

class ClashBasedShellSelectorCreator : public core::select::residue_selector::ResidueSelectorCreator {
public:
	select::residue_selector::ResidueSelectorOP create_residue_selector() const override;
	std::string keyname() const override;
	void provide_xml_schema( utility::tag::XMLSchemaDefinition & ) const override;
};

} //namespace residue_selector
} //namespace task
} //namespace pack
} //namespace core


#endif
