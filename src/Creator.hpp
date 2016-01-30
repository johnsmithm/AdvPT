#ifndef _CREATOR_H_
#define _CREATOR_H_

#include <list>
#include <string>
#include <vector>
#include <algorithm>
#include <list>
#include <iterator>
#include <random>
#include "GameObject.h"

#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

using namespace std;

enum class OptimizationMode {
    RUSH,
    PUSH
};

class Creator{
public:

	Creator(string targetUnit, OptimizationMode modeC);

	void createInitialBuildList(string target, vector<list<string>>& buildLists);

	void createNextGeneration(vector<list<string>> curGen, vector<list<string>>& nextGen);


public:
	static void getDeeperDependencies(string target, vector<list<string>>& deeperDependencies);

	bool gasNeeded(string name);

	int getDistance (list<string>& a,list<string>& b);

	bool checkValidity(list<string>& list, string newOne);

	bool checkBuildLists(list<string> listL);


	void getChild(list<string>& a, list<string>& b, list<string>& newList);

	vector<list<string>> nPointsCrossover(list<string> a,list<string> b, size_t n);
	list<string> nLengthCrossover(list<string> a,list<string> b, int n);//Todo-same length for each slice
	vector<string> nSlicesCrossover(vector<string> a,vector<string> b, int n);//Todo-different points for spliting

	void switchGenesMutation(vector<list<string>> &lists);//Todo-change two genes
	void deleteAddGenesMutation(vector<list<string>> &lists);//Todo


	void mutate(vector<list<string>> &lists);
	void reproduce(vector<list<string>>& bestLists, vector<list<string>>& children);

	bool meetGoal(list<string> newList, list<string> oldList);

	int supplyCheck;
	Race targetRace;
	string targetUnit;
	OptimizationMode modeC;

	string baseworker;
    string baseBuilding;
    string gasMaker;
    string supplyBuilding;

    vector<string> involvedUnits;

    std::default_random_engine randgen;
};

#endif
