#ifndef _CREATOR_H_
#define _CREATOR_H_

#include <deque>
#include <string>
#include <vector>

#include "GameObject.h"

using namespace std;

class Creator{
public:

	Creator(Race targetRace) : targetRace(targetRace){};

	static vector<deque<string>> createInitialBuildList(string target);

	void mutateBuildLists(vector<deque<string>> &lists);

	vector<vector<string>> reproductionDistance(vector<vector<string>> bestLists);

private:
	static vector<deque<string>> getDeeperDependencies(string target);

	int getDistance (vector<string> a,vector<string> b);

	bool checkValidity(vector<string> list, string newOne);

	vector<string> getChild(vector<string> a,vector<string> b);

	int supplyCheck;
	Race targetRace;
};

#endif