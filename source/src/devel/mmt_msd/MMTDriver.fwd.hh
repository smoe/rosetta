// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   devel/mmt_msd/MMTDriver.hh
/// @brief  declaration for class MMTDriver
/// @author Andrew Leaver-Fay (aleaverfay@gmail.com)

#ifndef INCLUDED_devel_mmt_msd_MMTDriver_hh
#define INCLUDED_devel_mmt_msd_MMTDriver_hh

// Utility headers
#include <utility/pointer/owning_ptr.hh>

namespace devel {
namespace mmt_msd {

class JobsForSequence;

typedef utility::pointer::shared_ptr< JobsForSequence > JobsForSequenceOP;
typedef utility::pointer::shared_ptr< JobsForSequence const > JobsForSequenceCOP;

class OneGenerationJobInfo;

typedef utility::pointer::shared_ptr< OneGenerationJobInfo > OneGenerationJobInfoOP;
typedef utility::pointer::shared_ptr< OneGenerationJobInfo const > OneGenerationJobInfoCOP;

class MMTDriver;

typedef utility::pointer::shared_ptr< MMTDriver > MMTDriverOP;
typedef utility::pointer::shared_ptr< MMTDriver const > MMTDriverCOP;

}
}

#endif
