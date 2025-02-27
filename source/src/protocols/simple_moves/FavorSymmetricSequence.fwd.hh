// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file protocols/simple_moves/FavorSymmetricSequence.fwd.hh
/// @brief apply constraints to enforce a symmetric sequence
/// @author Sam DeLuca

#ifndef INCLUDED_protocols_simple_moves_FavorSymmetricSequence_FWD_HH
#define INCLUDED_protocols_simple_moves_FavorSymmetricSequence_FWD_HH

// Utility headers
#include <utility/pointer/owning_ptr.hh>

namespace protocols {
namespace simple_moves {

class FavorSymmetricSequence;
typedef utility::pointer::shared_ptr< FavorSymmetricSequence > FavorSymmetricSequenceOP;
typedef utility::pointer::shared_ptr< FavorSymmetricSequence const > FavorSymmetricSequenceCOP;

}
}


#endif /* FAVORSYMMETRICSEQUENCE_HH_ */
