/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    The file with TreeAutomaton class.
 *
 *****************************************************************************/

#ifndef _SFTA_TREE_AUTOMATON_HH_
#define _SFTA_TREE_AUTOMATON_HH_

// SFTA header files
#include <sfta/abstract_transition_function.hh>
#include <sfta/vector.hh>

// Standard library header files
#include <set>

// Loki header files
#include <loki/SmartPtr.h>


// insert the class into proper namespace
namespace SFTA
{
	template
	<
		typename Symbol,
		typename State,
		class TransitionFunction,
		template <typename, typename> class StateTranslator
	>
	class TreeAutomaton;
}


template
<
	typename Symbol,
	typename State,
	class TransitionFunction,
	template <typename, typename> class StateTranslator
>
class SFTA::TreeAutomaton
	: private StateTranslator
		<
			State,
			typename TransitionFunction::StateType
		>
{
public:   // Public data types

	typedef Symbol SymbolType;
	typedef State AStateType;

	typedef typename TransitionFunction::StateType TFStateType;

	typedef TransitionFunction TransitionFunctionType;
	typedef typename TransitionFunctionType::TransitionListType TransitionListType;

	// uses ref counting
	typedef Loki::SmartPtr<TransitionFunctionType> TransFuncPtrType;

	typedef SFTA::Vector<AStateType> RuleLeftHandSideType;

	typedef std::set<AStateType> SetOfStatesType;

private:  // Private data types

	typedef SFTA::Private::Convert Convert;

	typedef StateTranslator
	<
		AStateType,
		TFStateType
	>
	ST;

	typedef std::set<TFStateType> SetOfTFStates;

private:  // Private data members

	TransFuncPtrType transFunc_;

	typename TransitionFunctionType::RegistrationTokenType regToken_;

	SetOfTFStates finalStates_;

public:   // Public methods

	TreeAutomaton()
		: transFunc_(new TransitionFunctionType()),
		  regToken_(),
			finalStates_()
	{
		regToken_ = transFunc_->RegisterAutomaton();
	}



	TreeAutomaton(TransFuncPtrType transFunction)
		: transFunc_(transFunction),
		  regToken_()
	{
		regToken_ = transFunc_->RegisterAutomaton();
	}


	void AddState(const AStateType& state)
	{
		TFStateType tfState = transFunc_->AllocateState(regToken_);
		ST::AddTranslation(state, tfState);
	}


	void AddTransition(const SymbolType& symbol,
		const RuleLeftHandSideType& lhs, const SetOfStatesType& rhs)
	{
		typename TransitionFunctionType::LeftHandSideType tfLhs(lhs.size());
		for (size_t i = 0; i < lhs.size(); ++i)
		{	// transform all states in LHS to their respective equivalent in the TF
			tfLhs[i] = ST::TranslateA2TF(lhs[i]);
		}

		typename TransitionFunctionType::InputRightHandSideType tfRhs;
		for (typename SetOfStatesType::const_iterator it = rhs.begin();
			it != rhs.end(); ++it)
		{	// transform all states in RHS to their respective equivalent in the TF
			tfRhs.push_back(ST::TranslateA2TF(*it));
		}

		transFunc_->AddTransition(regToken_, symbol, tfLhs, tfRhs);
	}

	void SetStateFinal(const AStateType& state)
	{
		TFStateType tfState = ST::TranslateA2TF(state);
		finalStates_.insert(tfState);
	}


	inline TransFuncPtrType GetTransitionFunction()
	{
		return transFunc_;
	}

	inline typename TransitionFunctionType::RegistrationTokenType
		GetRegToken() const
	{
		return regToken_;
	}

	std::string ToString()
	{
		typename TransitionFunctionType::TransitionListType lst
			= transFunc_->GetListOfTransitions(regToken_);

		std::string result;

		for (typename TransitionListType::const_iterator it = lst.begin();
			it != lst.end(); ++it)
		{	// for each transition of the transition function
			result += Convert::ToString(Loki::Field<0>(*it)) + "(";
			typename TransitionFunctionType::LeftHandSideType lhs
				= Loki::Field<1>(*it);

			if (lhs.size() > 0)
			{	// if the symbol is more than nullary
				//result += Convert::ToString(ST::TranslateTF2A(lhs[0]));
				result += Convert::ToString(lhs[0]);

				for (size_t i = 1; i < lhs.size(); ++i)
				{
					//result += ", " + Convert::ToString(ST::TranslateTF2A(lhs[i]));
					result += ", " + Convert::ToString(lhs[i]);
				}
			}

			const typename TransitionFunctionType::InputRightHandSideType& rhs
				= Loki::Field<2>(*it);

			// assertion
			assert(rhs.size() > 0);

			result += ") -> {";
			//result += Convert::ToString(ST::TranslateTF2A(rhs[0]));
			result += Convert::ToString(rhs[0]);
			for (size_t i = 1; i < rhs.size(); ++i)
			{
				//result += ", " + Convert::ToString(ST::TranslateTF2A(rhs[i]));
				result += ", " + Convert::ToString(rhs[i]);
			}

			result += "}\n";
		}

		return result;
	}


	~TreeAutomaton()
	{
		transFunc_->UnregisterAutomaton(regToken_);
	}



};

#endif
