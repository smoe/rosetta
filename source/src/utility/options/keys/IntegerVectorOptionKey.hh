// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   utility/options/keys/IntegerVectorOptionKey.hh
/// @brief  Automatic hidden index key for integer options
/// @author Stuart G. Mentzer (Stuart_Mentzer@objexx.com)


#ifndef INCLUDED_utility_options_keys_IntegerVectorOptionKey_hh
#define INCLUDED_utility_options_keys_IntegerVectorOptionKey_hh


// Unit headers
#include <utility/options/keys/IntegerVectorOptionKey.fwd.hh>

// Package headers
#include <utility/options/keys/VectorOptionKey.hh>
#include <utility/options/IntegerVectorOption.fwd.hh>

// Project headers
#include <utility/keys/SmallKeyVector.fwd.hh>


#ifdef    SERIALIZATION
// Cereal headers
#include <cereal/types/polymorphic.fwd.hpp>
#endif // SERIALIZATION

namespace utility {
namespace options {


/// @brief Automatic hidden index key for integer options
class IntegerVectorOptionKey :
	public VectorOptionKey
{


private: // Types


	typedef  VectorOptionKey  Super;


private: // Friends


	friend class utility::keys::SmallKeyVector< IntegerVectorOptionKey, IntegerVectorOption >;


public: // Creation


	/// @brief Default constructor
	inline
	IntegerVectorOptionKey()
	{}


	/// @brief Copy + identifier constructor
	inline
	IntegerVectorOptionKey(
		IntegerVectorOptionKey const & key,
		std::string const & id_a,
		std::string const & identifier_a = std::string(),
		std::string const & code_a = std::string()
	) :
		Super( key, id_a, identifier_a, code_a )
	{
		Lookup::add( *this ); // Add key to lookup map
	}


	/// @brief Key constructor
	inline
	explicit
	IntegerVectorOptionKey( Key const & key ) :
		Super( key )
	{}


	/// @brief Key + identifier constructor
	inline
	IntegerVectorOptionKey(
		Key const & key,
		std::string const & id_a,
		std::string const & identifier_a = std::string(),
		std::string const & code_a = std::string()
	) :
		Super( key, id_a, identifier_a, code_a )
	{
		Lookup::add( *this ); // Add key to lookup map
	}


	/// @brief Identifier constructor
	inline
	explicit
	IntegerVectorOptionKey(
		std::string const & id_a,
		std::string const & identifier_a = std::string(),
		std::string const & code_a = std::string()
	) :
		Super( id_a, identifier_a, code_a )
	{
		Lookup::add( *this ); // Add key to lookup map
	}


	/// @brief Clone this
	IntegerVectorOptionKey *
	clone() const override
	{
		return new IntegerVectorOptionKey( *this );
	}


	/// @brief Destructor
	inline
	~IntegerVectorOptionKey() override {}


public: // Assignment


	/// @brief Key assignment
	inline
	IntegerVectorOptionKey &
	operator =( Key const & key )
	{
		assign_Key( key );
		return *this;
	}


#ifdef    SERIALIZATION
public:
	template< class Archive > void save( Archive & arc ) const;
	template< class Archive > void load( Archive & arc );
#endif // SERIALIZATION

}; // IntegerVectorOptionKey


} // namespace options
} // namespace utility


#ifdef    SERIALIZATION
CEREAL_FORCE_DYNAMIC_INIT( utility_options_keys_IntegerVectorOptionKey )
#endif // SERIALIZATION


#endif // INCLUDED_utility_options_keys_IntegerVectorOptionKey_HH
