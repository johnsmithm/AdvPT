#ifndef _CREATOR_H_
#define _CREATOR_H_

#include <list>
#include <string>
#include <vector>
#include <algorithm>

#include "GameObject.h"

using namespace std;

class Creator{
public:

	Creator(Race targetRace) : targetRace(targetRace){};

	static void createInitialBuildList(string target, vector<list<string>>& buildLists);

	void createNextGeneration(vector<list<string>> curGen, vector<list<string>>& nextGen);

private:
	static void getDeeperDependencies(string target, vector<list<string>>& deeperDependencies);

	int getDistance (list<string>& a,list<string>& b);

	bool checkValidity(list<string>& list, string newOne);

	bool checkBuildLists(list<string> listL);

	void getChild(list<string>& a, list<string>& b, list<string>& newList);

	vector<string> nPointsCrossover(vector<string> a,vector<string> b, size_t n);
	vector<string> nLengthCrossover(vector<string> a,vector<string> b, int n);//Todo-same length for each slice
	vector<string> nSlicesCrossover(vector<string> a,vector<string> b, int n);//Todo-different points for spliting

	void switchGenesMutation(vector<list<string>> &lists);//Todo-change two genes
	void deleteAddGenesMutation(vector<list<string>> &lists);//Todo

	void mutate(vector<list<string>> &lists);
	void reproduce(vector<list<string>>& bestLists, vector<list<string>>& children);

	int supplyCheck;
	Race targetRace;
};

#endif