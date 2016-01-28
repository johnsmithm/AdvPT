#ifndef _SELECTOR_H_
#define _SELECTOR_H_

#include <functional>

#include <vector>
#include <utility>
#include <climits>
#include "Game.h"

#include "util.h"
#include "json/json.h"
#include "fstream"

using namespace std;
template<typename Gametype>
class Selector{
	public:
		Selector(string mode, string setTarget);
    	void getBestBuildLists(vector<vector<string>>& newlists, vector<pair<vector<string>, int>>& bestLists);
    	
	private:
		//vector<pair<vector<string>, int>> lastBuildLists;
		int getCompareCriteria(Json::Value output);
		string mode;
		string target;
		size_t arraySize;

};

/** @brief returns the fitness of a buildList from the output of its simulation
 *
 *  The smaller the output the fitter the buildList. Invalid build-lists return INT_MAX
 */
template<typename Gametype>
int Selector<Gametype>::getCompareCriteria(Json::Value output){
	if(mode == "rush"){
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
Selector<Gametype>::Selector(string setMode, string setTarget)
:mode(setMode), target(setTarget) ,arraySize(10){}

/**
 * Simulate newlists, add best list in the bestLists.
 */
template<typename Gametype>
void Selector<Gametype>::getBestBuildLists(vector<vector<string>>& newlists, vector<pair<vector<string>, int>>& bestLists){
	//lastBuildLists = bestLists;
	for(auto list : newlists){
		Gametype g;
		g.readBuildList(list);//TODO
		g.simulate();
		Json::Value output = g.getOutput().getJson();
		g.deleteInstanaces();
		if(output["buildlistValid"] == 1 && output["messages"].size() > 0){

			int compareCriteria = getCompareCriteria(output);
			if(bestLists.size() > arraySize ){
				if(bestLists[arraySize - 1].second <= compareCriteria)continue;
				bestLists.erase(bestLists.begin() + arraySize - 1);

			}

			size_t position = bestLists.size();
			// Find the position where buildList fits(Could use binary search).
			while(position > 0 && bestLists[--position].second > compareCriteria);
			if(position == 0){
				ofstream out("output.txt");
						out<<output;
			}
			bestLists.insert(bestLists.begin() + position, make_pair(list,compareCriteria));
		}
	}
}

#endif