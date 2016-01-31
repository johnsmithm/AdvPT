#ifndef _SELECTOR_H_
#define _SELECTOR_H_

#include <functional>
#include <mutex>
#include <queue>
#include <list>
#include <thread>
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
		int getCompareCriteria(Game& game);
		void threaded_evalution(vector<list<string>>& newlists,
		                         size_t startIndex,
		                         size_t stepsize,
		                         list<pair<list<string>, int>>& bestLists);

		OptimizationMode mode;
		string target;
		size_t arraySize;
		std::mutex evaluation_mutex;

		Creator creator;
};


#endif
