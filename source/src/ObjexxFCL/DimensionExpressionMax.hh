#ifndef INCLUDED_ObjexxFCL_DimensionExpressionMax_hh
#define INCLUDED_ObjexxFCL_DimensionExpressionMax_hh


// DimensionExpressionMax: DimensionExpression Binary Max Function
//
// Project: Objexx Fortran Compatibility Library (ObjexxFCL)
//
// Version: 3.0.0
//
// Language: C++
//
// Copyright (c) 2000-2009 Objexx Engineering, Inc. All Rights Reserved.
// Use of this source code or any derivative of it is restricted by license.
// Licensing is available from Objexx Engineering, Inc.:  http://objexx.com  Objexx@objexx.com


// ObjexxFCL Headers
#include <ObjexxFCL/DimensionExpression.hh>
#include <ObjexxFCL/DimensionExpressionCon.hh>

// C++ Headers
#include <algorithm>


namespace ObjexxFCL {


/// @brief DimensionExpressionMax: DimensionExpression Binary Max Function
class DimensionExpressionMax :
	public DimensionExpression
{


private: // Types


	typedef  DimensionExpression  Super;


public: // Creation


	/// @brief Copy Constructor
	inline
	DimensionExpressionMax( DimensionExpressionMax const & exp ) :
		Super(),
		exp1_p_( exp.exp1_p_ ? exp.exp1_p_->clone() : nullptr ),
		exp2_p_( exp.exp2_p_ ? exp.exp2_p_->clone() : nullptr )
	{
		assert( exp1_p_ );
		assert( exp2_p_ );
	}


	/// @brief Expression Constructor
	inline
	DimensionExpressionMax( DimensionExpression const & exp1, DimensionExpression const & exp2 ) :
		exp1_p_( exp1.clone() ),
		exp2_p_( exp2.clone() )
	{
		assert( exp1_p_ );
		assert( exp2_p_ );
	}


	/// @brief Expression Pointer Constructor (Ownership Transfer)
	inline
	DimensionExpressionMax( DimensionExpression * exp1_p_a, DimensionExpression * exp2_p_a ) :
		exp1_p_( exp1_p_a ),
		exp2_p_( exp2_p_a )
	{
		assert( exp1_p_ );
		assert( exp2_p_ );
	}


	/// @brief Clone
	inline
	DimensionExpression *
	clone() const override
	{
		assert( exp1_p_ );
		assert( exp2_p_ );
		if ( constant() ) {
			if ( integer() ) {
				return new DimensionExpressionCon( std::max( exp1_p_->ivalue(), exp2_p_->ivalue() ) );
			} else {
				return new DimensionExpressionCon( std::max( exp1_p_->value(), exp2_p_->value() ) );
			}
		} else {
			return new DimensionExpressionMax( exp1_p_->clone(), exp2_p_->clone() );
		}
	}


	/// @brief Clone with Dimension Substitution
	inline
	DimensionExpression *
	clone( Dimension const & dim ) const override
	{
		assert( exp1_p_ );
		assert( exp2_p_ );
		if ( constant() ) {
			if ( integer() ) {
				return new DimensionExpressionCon( std::max( exp1_p_->ivalue(), exp2_p_->ivalue() ) );
			} else {
				return new DimensionExpressionCon( std::max( exp1_p_->value(), exp2_p_->value() ) );
			}
		} else {
			return new DimensionExpressionMax( exp1_p_->clone( dim ), exp2_p_->clone( dim ) );
		}
	}


	/// @brief Destructor
	inline
	~DimensionExpressionMax() override
	{
		assert( exp1_p_ );
		assert( exp2_p_ );
		delete exp1_p_;
		delete exp2_p_;
	}


public: // Inspector


	/// @brief Initialized?
	inline
	bool
	initialized() const override
	{
		assert( exp1_p_ );
		assert( exp2_p_ );
		return ( ( exp1_p_->initialized() ) && ( exp2_p_->initialized() ) );
	}


	/// @brief Integer?
	inline
	bool
	integer() const override
	{
		assert( exp1_p_ );
		assert( exp2_p_ );
		return ( ( exp1_p_->integer() ) && ( exp2_p_->integer() ) );
	}


	/// @brief Constant?
	inline
	bool
	constant() const override
	{
		assert( exp1_p_ );
		assert( exp2_p_ );
		return ( ( exp1_p_->constant() ) && ( exp2_p_->constant() ) );
	}


	/// @brief Reference?
	inline
	bool
	reference() const override
	{
		assert( exp1_p_ );
		assert( exp2_p_ );
		return ( ( exp1_p_->reference() ) || ( exp2_p_->reference() ) );
	}


	/// @brief Reducible?
	inline
	bool
	reducible() const override
	{
		assert( exp1_p_ );
		assert( exp2_p_ );
		return ( ( constant() ) || ( exp1_p_->reducible() ) || ( exp2_p_->reducible() ) );
	}


	/// @brief Value
	inline
	double
	operator ()() const override
	{
		assert( exp1_p_ );
		assert( exp2_p_ );
		return ( std::max( exp1_p_->operator ()(), exp2_p_->operator ()() ) );
	}


	/// @brief Value
	inline
	double
	value() const override
	{
		assert( exp1_p_ );
		assert( exp2_p_ );
		return ( std::max( exp1_p_->value(), exp2_p_->value() ) );
	}


	/// @brief Insert an Observer
	inline
	void
	insert_observer( Observer & observer ) const override
	{
		assert( exp1_p_ );
		assert( exp2_p_ );
		exp1_p_->insert_observer( observer );
		exp2_p_->insert_observer( observer );
	}


	/// @brief Remove an Observer
	inline
	void
	remove_observer( Observer & observer ) const override
	{
		assert( exp1_p_ );
		assert( exp2_p_ );
		exp1_p_->remove_observer( observer );
		exp2_p_->remove_observer( observer );
	}


public: // Modifier


	/// @brief Update for Destruction of a Subject
	inline
	void
	destructed( Subject const & subject ) override
	{
		assert( exp1_p_ );
		assert( exp2_p_ );
		exp1_p_->destructed( subject );
		exp2_p_->destructed( subject );
	}


private: // Data


	/// @brief Pointer to expression 1
	DimensionExpression * exp1_p_;

	/// @brief Pointer to expression 2
	DimensionExpression * exp2_p_;


}; // DimensionExpressionMax


} // namespace ObjexxFCL


#endif // INCLUDED_ObjexxFCL_DimensionExpressionMax_HH
