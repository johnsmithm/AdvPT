#ifndef _CREATOR_H_
#define _CREATOR_H_

#include <deque>
#include <string>
#include <vector>
#include <algorithm>
#include <list>
#include <iterator>
#include "GameObject.h"

using namespace std;

class Creator{
public:

	Creator(Race targetRace) : targetRace(targetRace){};

	static void createInitialBuildList(string target, vector<deque<string>>& buildLists);

	void mutateBuildLists(vector<deque<string>> &lists);
private:
	static void getDeeperDependencies(string target, vector<deque<string>>& deeperDependencies);

	int getDistance (list<string>& a,list<string>& b);

	bool checkValidity(list<string>& list, string newOne);

	bool checkBuildLists(list<string> list);

	void getChild(list<string>& a, list<string>& b, list<string>& newList);

	list<string> nPointsCrossover(list<string> a,list<string> b, size_t n);
	list<string> nLengthCrossover(list<string> a,list<string> b, int n);//Todo-same length for each slice
	vector<string> nSlicesCrossover(vector<string> a,vector<string> b, int n);//Todo-different points for spliting

	void switchGenesMutation(vector<deque<string>> &lists);//Todo-change two genes
	void deleteAddGenesMutation(vector<deque<string>> &lists);//Todo

	void reproduce(vector<list<string>>& bestLists, vector<list<string>>& children);	

	int supplyCheck;
	Race targetRace;
};

#endif