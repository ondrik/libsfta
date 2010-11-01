/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Source file for program that performs union of two automata.
 *
 *****************************************************************************/

// Standard library headers
#include <cstdlib>
#include <fstream>
#include <iostream>

// SFTA library headers
#include <sfta/bu_tree_automaton_cover.hh>
#include <sfta/ta_building_director.hh>
#include <sfta/timbuk_ta_builder.hh>



typedef SFTA::BUTreeAutomatonCover<64> TreeAutomaton;

typedef SFTA::TABuildingDirector<TreeAutomaton> TABuildingDirector;
typedef SFTA::AbstractTABuilder<TreeAutomaton> AbstractTABuilder;
typedef SFTA::TimbukTABuilder<TreeAutomaton> TimbukTABuilder;


void printHelp(const std::string& programName)
{
	std::cout << "Invalid arguments.\n";
	std::cout << "Run:  " << programName << " <file1> <file2>\n";
	std::cout << "where <file1> and <file2> are files with tree automata in Timbuk format.\n";
}


int main(int argc, char* argv[])
{
	// Assertions
	assert(argc >= 1);

	if (argc != 3)
	{
		printHelp(argv[0]);
		return EXIT_FAILURE;
	}

	std::ifstream ifs(argv[1]);
	if (ifs.fail())
	{
		std::cout << "Could not open file " << argv[1] << "!\n";
		return EXIT_FAILURE;
	}

	AbstractTABuilder* builder = new TimbukTABuilder();
	TABuildingDirector director(builder);

	TreeAutomaton* ta1 = director.Construct(ifs);

//	BUTreeAutomatonCover* aut1 = director.Construct();
//
	delete ta1;
	delete builder;

	return EXIT_SUCCESS;
}