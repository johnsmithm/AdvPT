#ifndef _SELECTOR_H_
#define _SELECTOR_H_

#include <functional>
#include <queue>
#include <list>
#include <vector>
#include <utility>
#include <climits>

#include "Creator.hpp"
#include "Game.h"
#include "util.h"
#include "json/json.h"
#include "fstream"

using namespace std;


class ListComparator {
public:
	bool operator()(const pair<list<string>, int> &a, const pair<list<string>, int> &b){
		//TODO: Figure out, if this is the correct ordering.
		//pop_back() should remove lowest fitness => order in ascending fitness
		return a.second < b.second;
	}
};

template<typename Gametype>
class Selector{
	public:
		Selector(OptimizationMode mode, string setTarget);
		void getBestBuildLists(vector<list<string>>& newlists, list<pair<list<string>, int>>& bestLists);
		void optimize(int maxIterations);

	private:
		//vector<pair<vector<string>, int>> lastBuildLists;
		int getCompareCriteria(Json::Value output);
		OptimizationMode mode;
		string target;
		size_t arraySize;

		Creator creator;
};


#endif