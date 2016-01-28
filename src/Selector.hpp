#ifndef _SELECTOR_H_
#define _SELECTOR_H_

#include <functional>
#include <queue>
#include <deque>
#include <vector>
#include <utility>
#include <climits>
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
		//pop() should remove lowest fitness => order in descending fitness
		return a.second < b.second;
	}
};

template<typename Gametype>
class Selector{
	public:
		Selector(OptimizationMode mode, string setTarget);
		void getBestBuildLists(vector<deque<string>>& newlists);

	private:
		//vector<pair<vector<string>, int>> lastBuildLists;
		int getCompareCriteria(Json::Value output);
		OptimizationMode mode;
		string target;
		size_t arraySize;
		priority_queue<pair<deque<string>, int>, deque<pair<deque<string>, int>>, ListComparator> bestLists;

};

/** @brief returns the fitness of a buildList from the output of its simulation
 *
 *  The smaller the output the fitter the buildList. Invalid build-lists return INT_MAX
 */
template<typename Gametype>
int Selector<Gametype>::getCompareCriteria(Json::Value output){
	if(mode == OptimizationMode::RUSH){
		if(output["buildlistValid"] == 1 && output["messages"].size() > 0){
			Json::Value maX = 0;
			return output["messages"][output["messages"].size()-1]["time"].asInt();
		}
		return INT_MAX;
	}
	else {
		if(output["buildlistValid"] == 1 && output["messages"].size() > 0){
			int count = 0;
			if(output["messages"][output["messages"].size()-1]["time"].asInt() > 360)
				return INT_MAX;
			for(auto message : output["messages"])
				for(auto event : message["events"])
					if(event["type"] == "build-start" && event["name"] == target)
						++count;
			return -count;
		}
		return INT_MAX;
	}
}

template<typename Gametype>
Selector<Gametype>::Selector(OptimizationMode setMode, string setTarget)
:mode(setMode), target(setTarget), arraySize(10){}

/**
 * Simulate newlists, add best list in the bestLists.
 */
template<typename Gametype>
void Selector<Gametype>::getBestBuildLists(vector<deque<string>>& newlists){
	//lastBuildLists = bestLists;
	for(auto list : newlists){
		Gametype g;
		g.readBuildList(list);
		g.simulate();
		Json::Value output = g.getOutput().getJson();
		g.deleteInstanaces();
		if(output["buildlistValid"] == 1 && output["messages"].size() > 0){

			int compareCriteria = getCompareCriteria(output);

			bestLists.push(make_pair(list,compareCriteria));

			while(bestLists.size() > arraySize)
				bestLists.pop();
		}
	}
}

#endif