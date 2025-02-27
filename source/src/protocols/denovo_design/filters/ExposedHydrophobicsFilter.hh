// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file src/protocols/denovo_design/filters/ExposedHydrophobicsFilter.hh
/// @brief Tom's Denovo Protocol. This is freely mutable and used for playing around with stuff
/// @details
/// @author Tom Linsky (tlinsky@gmail.com)


#ifndef INCLUDED_protocols_denovo_design_filters_ExposedHydrophobicsFilter_hh
#define INCLUDED_protocols_denovo_design_filters_ExposedHydrophobicsFilter_hh

// Unit headers
#include <protocols/denovo_design/filters/ExposedHydrophobicsFilter.fwd.hh>

// Project headers
#include <protocols/filters/Filter.hh>


#include <core/pose/Pose.fwd.hh>


//// C++ headers
#include <string>



namespace protocols {
namespace denovo_design {
namespace filters {

class ExposedHydrophobicsFilter : public protocols::filters::Filter {
public:

	/// @brief Initialize ExposedHydrophobicsFilter
	ExposedHydrophobicsFilter();

	/// @brief virtual constructor to allow derivation
	~ExposedHydrophobicsFilter() override;

	/// @brief Parses the ExposedHydrophobicsFilter tags
	void parse_my_tag(
		utility::tag::TagCOP tag,
		basic::datacache::DataMap & data
	) override;

	/// @brief Return the name of this mover.
	virtual std::string get_name() const;

	/// @brief return a fresh instance of this class in an owning pointer
	protocols::filters::FilterOP clone() const override;

	/// @brief Apply the ExposedHydrophobicsFilter. Overloaded apply function from filter base class.
	bool apply( core::pose::Pose const & pose ) const override;
	protocols::filters::FilterOP fresh_instance() const override;
	void report( std::ostream & out, core::pose::Pose const & pose ) const override;
	core::Real report_sm( core::pose::Pose const & pose ) const override;
	core::Real compute( core::pose::Pose const & pose ) const;

	std::string
	name() const override;

	static
	std::string
	class_name();

	static
	void
	provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd );

	/// @brief Sets the threshold; if the calculated filters core is <= threshold, the filter passes, otherwise it fails
	void
	set_threshold( core::Real const threshold ) { threshold_ = threshold; }

	/// @brief Sets the SASA cutoff in order for a residue to be considered buried. If a residue has sasa <= sasa_cutoff, it is considered buried and will be ignored by the filter
	void
	set_sasa_cutoff( core::Real const sasa_cutoff ) { sasa_cutoff_ = sasa_cutoff; }

private:   // options
	/// @brief If total calculated filter score is <= theshold_, the filter passes, otherwise it fails.
	core::Real threshold_;
	/// @brief If a residue has sasa <= sasa_cutoff_, it is considered buried and ignored
	core::Real sasa_cutoff_;

private:   // other data
	static std::string const hydrophobic_residues_;

};


} // filters
} // denovo_design
} // protocols

#endif
