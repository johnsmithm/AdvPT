#ifndef _CREATOR_H_
#define _CREATOR_H_

#include <deque>
#include <string>
#include <vector>
#include <algorithm>

#include "GameObject.h"

using namespace std;

class Creator{
public:

	Creator(Race targetRace) : targetRace(targetRace){};

	static void createInitialBuildList(string target, vector<deque<string>>& buildLists);

	void createNextGeneration(vector<deque<string>> curGen, vector<deque<string>>& nextGen);
private:
	static void getDeeperDependencies(string target, vector<deque<string>>& deeperDependencies);

	int getDistance (deque<string>& a,deque<string>& b);

	bool checkValidity(deque<string>& list, string newOne);

	bool checkBuildList(deque<string> list);

	void getChild(deque<string>& a, deque<string>& b, deque<string>& newList);

	vector<string> nPointsCrossover(vector<string> a,vector<string> b, size_t n);
	vector<string> nLengthCrossover(vector<string> a,vector<string> b, int n);//Todo-same length for each slice
	vector<string> nSlicesCrossover(vector<string> a,vector<string> b, int n);//Todo-different points for spliting

	void switchGenesMutation(vector<deque<string>> &lists);//Todo-change two genes
	void deleteAddGenesMutation(vector<deque<string>> &lists);//Todo

	void mutate(vector<deque<string>> &lists);
	void reproduce(vector<deque<string>>& bestLists, vector<deque<string>>& children);

	int supplyCheck;
	Race targetRace;
};

#endif