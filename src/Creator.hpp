#ifndef _CREATOR_H_
#define _CREATOR_H_

#include <string>
#include <vector>

#include "GameObject.h"

using namespace std;

class Creator{
public:

	static vector<vector<string>> createInitialBuildList(string target);

	static void mutateBuildList(vector<vector<string>> &list);

private:
	static vector<vector<string>> getDeeperDependencies(string target);
};

#endif