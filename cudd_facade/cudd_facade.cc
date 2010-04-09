/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Implementation file of CUDDFacade. This file uses directly CUDD
 *    functions and provides their actions through the interface of
 *    CUDDFacade.
 *
 *****************************************************************************/


// Standard library headers
#include <stdexcept>

// SFTA headers
#include <sfta/cudd_facade.hh>
#include <sfta/sfta.hh>
#include <sfta/convert.hh>

// CUDD headers
#include <util.h>
#include <cudd.h>
#include <cuddInt.h>

using namespace SFTA::Private;


// Setting the logging category name for Log4cpp
const char* CUDDFacade::LOG_CATEGORY_NAME = "cudd_facade";


// A set of functions that converts between SFTA and CUDD manager and node
// types. 
namespace SFTA
{
	namespace Private
	{
		/**
		 * @brief  Converts manager from CUDD format
		 *
		 * Converts a pointer to manager from DdManager type to
		 * CUDDFacade::Manager type.
		 *
		 * @param[in]  cudd_value  CUDD DdManager pointer
		 *
		 * @returns  SFTA Manager pointer
		 */
		CUDDFacade::Manager* fromCUDD(DdManager* cudd_value)
		{
			return reinterpret_cast<CUDDFacade::Manager*>(cudd_value);
		}


		/**
		 * @brief  Converts node from CUDD format
		 *
		 * Converts a pointer to manager from DdNode type to
		 * CUDDFacade::Node type.
		 *
		 * @param[in]  cudd_value  CUDD DdNode pointer
		 *
		 * @returns  SFTA Node pointer
		 */
		CUDDFacade::Node* fromCUDD(DdNode* cudd_value)
		{
			return reinterpret_cast<CUDDFacade::Node*>(cudd_value);
		}


		/**
		 * @brief  Converts manager to CUDD format
		 *
		 * Converts a pointer to manager from CUDDFacade::Manager type to
		 * DdManager type.
		 *
		 * @param[in]  sfta_value  SFTA CUDDFacade::Manager pointer
		 *
		 * @returns  CUDD DdManager pointer
		 */
		DdManager* toCUDD(CUDDFacade::Manager* sfta_value)
		{
			return reinterpret_cast<DdManager*>(sfta_value);
		}


		/**
		 * @brief  Converts node to CUDD format
		 *
		 * Converts a pointer to manager from CUDDFacade::Node type to
		 * DdNode type.
		 *
		 * @param[in]  sfta_value  SFTA CUDDFacade::Node pointer
		 *
		 * @returns  CUDD DdNode pointer
		 */
		DdNode* toCUDD(CUDDFacade::Node* sfta_value)
		{
			return reinterpret_cast<DdNode*>(sfta_value);
		}
	}
}


CUDDFacade::CUDDFacade()
	: manager_(static_cast<Manager*>(0))
{
	// Create the manager
	if ((manager_ = fromCUDD(Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0)))
		== static_cast<Manager*>(0))
	{	// in case the manager could not be created
		std::string error_msg = "CUDD Manager could not be created";
		SFTA_LOGGER_FATAL(error_msg);
		throw std::runtime_error(error_msg);
	}
}


CUDDFacade::Node* CUDDFacade::AddIthVar(int i) const
{
	// Assertions
	assert(manager_ != static_cast<Manager*>(0));
	assert(i >= 0);

	Node* res = fromCUDD(Cudd_addIthVar(toCUDD(manager_), i));

	// check the return value
	assert(res != static_cast<Node*>(0));

	return res;
}


SFTA::Private::CUDDFacade::Node* CUDDFacade::AddCmpl(Node* node) const
{
	// Assertions
	assert(manager_ != static_cast<Manager*>(0));
	assert(node != static_cast<Node*>(0));

	Node* res = fromCUDD(Cudd_addCmpl(toCUDD(manager_), toCUDD(node)));

	// check the return value
	assert(res != static_cast<Node*>(0));

	return res;
}


CUDDFacade::Node* CUDDFacade::AddConst(CUDDFacade::ValueType value) const
{
	// Assertions
	assert(manager_ != static_cast<Manager*>(0));

	Node* res = fromCUDD(Cudd_addConst(toCUDD(manager_), value));

	// check the return value
	assert(res != static_cast<Node*>(0));

	return res;
}


void CUDDFacade::Ref(Node* node) const
{
	// Assertions
	assert(manager_ != static_cast<Manager*>(0));
	assert(node != static_cast<Node*>(0));

	Cudd_Ref(toCUDD(node));
}


void CUDDFacade::RecursiveDeref(Node* node) const
{
	// Assertions
	assert(manager_ != static_cast<Manager*>(0));
	assert(node != static_cast<Node*>(0));

	Cudd_RecursiveDeref(toCUDD(manager_), toCUDD(node));
}


void CUDDFacade::SetBackground(Node* bck) const
{
	// Assertions
	assert(manager_ != static_cast<Manager*>(0));
	assert(bck != static_cast<Node*>(0));

	Cudd_SetBackground(toCUDD(manager_), toCUDD(bck));
}


CUDDFacade::Node* CUDDFacade::ReadBackground() const
{
	// Assertions
	assert(manager_ != static_cast<Manager*>(0));

	Node* res = fromCUDD(Cudd_ReadBackground(toCUDD(manager_)));

	// check the return value
	assert(res != static_cast<Node*>(0));

	return res;
}


CUDDFacade::Node* CUDDFacade::Times(Node* lhs, Node* rhs) const
{
	// Assertions
	assert(manager_ != static_cast<Manager*>(0));
	assert(!(cuddIsConstant(toCUDD(lhs)) && cuddIsConstant(toCUDD(rhs))));

	Node* res = fromCUDD(Cudd_addApply(toCUDD(manager_), Cudd_addTimes,
		toCUDD(lhs), toCUDD(rhs)));

	// check the return value
	assert(res != static_cast<Node*>(0));

	return res;
}


DdNode* applyCallback(DdManager* dd, DdNode** f, DdNode** g, void* data)
{
	// Assertions
	assert(dd   != static_cast<DdManager*>(0));
	assert(f    != static_cast<DdNode**>(0));
	assert(g    != static_cast<DdNode**>(0));
	assert(data != static_cast<void*>(0));

	// get values of the nodes
	DdNode* F = *f;
	DdNode* G = *g;

	// Further assertions
	assert(F    != static_cast<DdNode*>(0));
	assert(G    != static_cast<DdNode*>(0));

	// get callback parameters from the data container
	CUDDFacade::ApplyCallbackParameters& params =
		*(static_cast<CUDDFacade::ApplyCallbackParameters*>(data));

	// Even further assertions
	assert(params.Op != static_cast<CUDDFacade::ApplyOperationType>(0));

	if (cuddIsConstant(F) && cuddIsConstant(G))
	{	// in case we are at leaves
		DdNode* res = cuddUniqueConst(dd,
			params.Op(cuddV(F), cuddV(G), params.Data));

		// check the return value
		assert(res != static_cast<DdNode*>(0));

		return res;
	}
	else
	{	// in case we are not at leaves
		return static_cast<DdNode*>(0);
	}
}


DdNode* monadicApplyCallback(DdManager* dd, DdNode * f, void* data)
{
	// Assertions
	assert(dd   != static_cast<DdManager*>(0));
	assert(f    != static_cast<DdNode*>(0));
	assert(data != static_cast<void*>(0));

	// get callback parameters from the data container
	CUDDFacade::MonadicApplyCallbackParameters& params =
		*(static_cast<CUDDFacade::MonadicApplyCallbackParameters*>(data));

	// Even further assertions
	assert(params.Op != static_cast<CUDDFacade::MonadicApplyOperationType>(0));

	if (cuddIsConstant(f))
	{	// in case we are at leaves
		DdNode* res = cuddUniqueConst(dd, params.Op(cuddV(f), params.Data));

		// check the return value
		assert(res != static_cast<DdNode*>(0));

		return(res);
	}
	else
	{	// in case we are not at leaves
		return static_cast<DdNode*>(0);
	}
}


CUDDFacade::Node* CUDDFacade::Apply(Node* lhs, Node* rhs,
	ApplyCallbackParameters* cbParams) const
{
	// Assertions
	assert(manager_ != static_cast<Manager*>(0));
	assert(lhs != static_cast<Node*>(0));
	assert(rhs != static_cast<Node*>(0));
	assert(cbParams != static_cast<ApplyCallbackParameters*>(0));

	Node* res = fromCUDD(Cudd_addApplyWithData(
		toCUDD(manager_), applyCallback, toCUDD(lhs), toCUDD(rhs), cbParams));

	// check the return value
	assert(res != static_cast<Node*>(0));

	return res;
}


CUDDFacade::Node* CUDDFacade::MonadicApply(Node* root,
	MonadicApplyCallbackParameters* cbParams) const
{
	// Assertions
	assert(manager_ != static_cast<Manager*>(0));
	assert(root != static_cast<Node*>(0));
	assert(cbParams != static_cast<MonadicApplyCallbackParameters*>(0));

	Node* res = fromCUDD(Cudd_addMonadicApplyWithData(
		toCUDD(manager_), monadicApplyCallback, toCUDD(root), cbParams));

	// check the return value
	assert(res != static_cast<Node*>(0));

	return res;
}


void CUDDFacade::DumpDot(const std::vector<Node*>& nodes,
		const std::vector<std::string>& inames,
		const std::vector<std::string>& onames,
		const std::string& filename) const
{
	// Assertions
	assert(manager_ != static_cast<Manager*>(0));
	assert((inames.size() == 0) || (nodes.size() == inames.size()));
	assert((onames.size() == 0) || (nodes.size() == onames.size()));

	// arrays to be passed to the CUDD function
	DdNode** arrNodes = static_cast<DdNode**>(0);
	char** arrInames = static_cast<char**>(0);
	char** arrOnames = static_cast<char**>(0);

	// the number of nodes
	unsigned size = nodes.size();

	SFTA_LOGGER_DEBUG("Dumping a diagram with "
		+ Convert::ToString(size) + " nodes");

	// the file that the BDD should be dumped to
	FILE* outfile = static_cast<FILE*>(0);

	try
	{	// try block due to possible memory errors
		if ((outfile = fopen(filename.c_str(), "w")) == static_cast<FILE*>(0))
		{	// in case the file could not be open
			throw std::runtime_error("Could not open file!");
		}

		// create the array of nodes
		arrNodes = new DdNode*[size];
		for (unsigned i = 0; i < size; ++i)
		{	// insert the nodes to the array
			arrNodes[i] = toCUDD(nodes[i]);
		}

		if (inames.size() > 0)
		{	// if there are names of roots
			arrInames = new char*[size];

			for (unsigned i = 0; i < size; ++i)
			{	// copy names of roots
				arrInames[i] = const_cast<char*>(inames[i].c_str());
			}
		}

		if (inames.size() > 0)
		{	// if there are names of sink nodes
			arrOnames = new char*[size];

			for (unsigned i = 0; i < size; ++i)
			{	// copy names of sink nodes
				arrOnames[i] = const_cast<char*>(onames[i].c_str());
			}
		}

		if (!(Cudd_DumpDot(
			toCUDD(manager_), size, arrNodes, arrInames, arrOnames, outfile)))
		{	// in case there was a problem with dumping the file
			throw std::runtime_error("Could not dump BDD!");
		}

		// delete the arrays
		delete [] arrNodes;
		arrNodes = static_cast<DdNode**>(0);
		delete [] arrInames;
		arrInames = static_cast<char**>(0);
		delete [] arrOnames;
		arrOnames = static_cast<char**>(0);

		if (!(fclose(outfile)))
		{	// in case everything was alright
			outfile = static_cast<FILE*>(0);
		}
		else
		{	// in case there was a problem with closing the file
			outfile = static_cast<FILE*>(0);
			throw std::runtime_error("Could not close the dump file!");
		}
	}
	catch (const std::exception& e)
	{	// in case an exception appears, we only log it
		if (outfile != static_cast<FILE*>(0))
		{	// in case the file has not been closed yet
			fclose(outfile);
			outfile = static_cast<FILE*>(0);
		}

		if (arrNodes != static_cast<DdNode**>(0))
		{	// in case the array of nodes is still allocated
			delete [] arrNodes;
			arrNodes = static_cast<DdNode**>(0);
		}

		if (arrInames != static_cast<char**>(0))
		{	// in case the array of root nodes' names is still allocated
			delete [] arrInames;
			arrInames = static_cast<char**>(0);
		}

		if (arrOnames != static_cast<char**>(0))
		{	// in case the array of sink nodes' names is still allocated
			delete [] arrOnames;
			arrOnames = static_cast<char**>(0);
		}

		SFTA_LOGGER_ERROR("Error while dumping BDD to file \"" + filename + "\": " + e.what());
	}
}


CUDDFacade::~CUDDFacade()
{
	// Assertions
	assert(manager_ != static_cast<Manager*>(0));

	// Derefence the background
	RecursiveDeref(ReadBackground());

	// Check for nodes with non-zero reference count
	int unrefed = 0;
	if ((unrefed = Cudd_CheckZeroRef(toCUDD(manager_))) != 0)
	{	// in case there are still some nodes unreferenced
		SFTA_LOGGER_WARN("Still " + Convert::ToString(unrefed) + " nodes unreferenced!");
	}

	// Delete the manager
	Cudd_Quit(toCUDD(manager_));
	manager_ = static_cast<Manager*>(0);
}
