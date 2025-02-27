// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file protocols/simple_moves/LoadUnboundRotMover.cc
/// @brief LoadUnboundRotMover methods implemented
/// @author Steven Lewis smlewi@gmail.com, Brian Weitzner brian.weitzner@gmail.com

// Unit Headers
#include <protocols/simple_moves/LoadUnboundRotMover.hh>
#include <protocols/simple_moves/LoadUnboundRotMoverCreator.hh>

// Package Headers
#include <core/pack/dunbrack/RotamerConstraint.hh>

// Project Headers
#include <core/pose/Pose.fwd.hh>

// Utility Headers
#include <basic/Tracer.hh>
// XSD XRW Includes
#include <utility/tag/XMLSchemaGeneration.hh>
#include <protocols/moves/mover_schemas.hh>

// C++ Headers

static basic::Tracer TR( "protocols.simple_moves.LoadUnboundRotMover" );

namespace protocols {
namespace simple_moves {




LoadUnboundRotMover::LoadUnboundRotMover()
: protocols::moves::Mover("LoadUnboundRotMover")
{}

LoadUnboundRotMover::~LoadUnboundRotMover()= default;

/// @details
void LoadUnboundRotMover::apply( core::pose::Pose & pose ){
	core::pack::dunbrack::load_unboundrot(pose);
	return;
}//apply


protocols::moves::MoverOP LoadUnboundRotMover::fresh_instance() const { return utility::pointer::make_shared< LoadUnboundRotMover >(); }
protocols::moves::MoverOP LoadUnboundRotMover::clone() const { return utility::pointer::make_shared< LoadUnboundRotMover >( *this ); }

/// @brief parse XML (specifically in the context of the parser/scripting scheme); it's a no-op
void
LoadUnboundRotMover::parse_my_tag(
	utility::tag::TagCOP const,
	basic::datacache::DataMap &
)
{}

std::string LoadUnboundRotMover::get_name() const {
	return mover_name();
}

std::string LoadUnboundRotMover::mover_name() {
	return "load_unbound_rot";
}

void LoadUnboundRotMover::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd )
{
	using namespace utility::tag;
	AttributeList attlist;
	protocols::moves::xsd_type_definition_w_attributes(
		xsd, mover_name(),
		"Add the rotamers of the specified structure to the rotamer library "
		"(usually used to include rotamers of unbound monomer)",
		attlist );
}

std::string LoadUnboundRotMoverCreator::keyname() const {
	return LoadUnboundRotMover::mover_name();
}

protocols::moves::MoverOP
LoadUnboundRotMoverCreator::create_mover() const {
	return utility::pointer::make_shared< LoadUnboundRotMover >();
}

void LoadUnboundRotMoverCreator::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd ) const
{
	LoadUnboundRotMover::provide_xml_schema( xsd );
}


}//simple_moves
}//protocols

