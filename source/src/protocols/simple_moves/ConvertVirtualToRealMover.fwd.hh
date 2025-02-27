// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available
// (c) under license. The Rosetta software is developed by the contributing
// (c) members of the Rosetta Commons. For more information, see
// (c) http://www.rosettacommons.org. Questions about this can be addressed to
// (c) University of Washington UW TechTransfer,email:license@u.washington.edu.

/// @file protocols/simple_moves/ConvertVirtualToRealMover.fwd.hh
/// @brief Mover for switching virtual residues back to real residues
/// @author raemisch (raemisch@scripps.edu)


#ifndef INCLUDED_protocols_simple_moves_VirtualToFaMover_fwd_hh
#define INCLUDED_protocols_simple_moves_VirtualToFaMover_fwd_hh

// Utility headers
#include <utility/pointer/owning_ptr.hh>



// Forward
namespace protocols {
namespace simple_moves {

class ConvertVirtualToRealMover;

typedef utility::pointer::shared_ptr< ConvertVirtualToRealMover > VirtualToFaMoverOP;
typedef utility::pointer::shared_ptr< ConvertVirtualToRealMover const > VirtualToFaMoverCOP;



} //protocols
} //simple_moves


#endif //INCLUDED_protocols_simple_moves_VirtualToFaMover_fwd_hh





