#ifndef _CREATOR_H_
#define _CREATOR_H_

#include <deque>
#include <string>
#include <vector>

#include "GameObject.h"

using namespace std;

class Creator{
public:

	static vector<deque<string>> createInitialBuildList(string target);

	static void mutateBuildLists(vector<deque<string>> &lists);

private:
	static vector<deque<string>> getDeeperDependencies(string target);
};

#endif