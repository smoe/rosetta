// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file core/pack/task/operation/EnableSmartAnnealer.fwd.hh
/// @author Jack Maguire

#ifndef INCLUDED_core_pack_task_operation_EnableSmartAnnealer_fwd_hh
#define INCLUDED_core_pack_task_operation_EnableSmartAnnealer_fwd_hh

// Utility headers
#include <utility/pointer/owning_ptr.hh>


// Forward
namespace core {
namespace pack {
namespace task {
namespace operation {

class EnableSmartAnnealer;

using EnableSmartAnnealerOP = utility::pointer::shared_ptr< EnableSmartAnnealer >;
using EnableSmartAnnealerCOP = utility::pointer::shared_ptr< EnableSmartAnnealer const >;

} //operation
} //task
} //pack
} //core

#endif //INCLUDED_core_pack_task_operation_EnableSmartAnnealer_fwd_hh
