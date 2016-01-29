#ifndef _SELECTOR_H_
#define _SELECTOR_H_

#include <functional>
#include <queue>
#include <deque>
#include <vector>
#include <utility>
#include <climits>

#include "Creator.hpp"
#include "Game.h"
#include "util.h"
#include "json/json.h"
#include "fstream"

using namespace std;

enum class OptimizationMode {
    RUSH,
    PUSH
};

class ListComparator {
public:
	bool operator()(const pair<deque<string>, int> &a, const pair<deque<string>, int> &b){
		//TODO: Figure out, if this is the correct ordering.
		//pop_back() should remove lowest fitness => order in ascending fitness
		return a.second > b.second;
	}
};

template<typename Gametype>
class Selector{
	public:
		Selector(OptimizationMode mode, string setTarget);
		void getBestBuildLists(vector<deque<string>>& newlists, list<pair<deque<string>, int>>& bestLists);
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