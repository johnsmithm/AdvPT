#include "Selector.hpp"


template<typename Gametype>
Selector<Gametype>::Selector(OptimizationMode setMode, string setTarget)
:mode(setMode), target(setTarget), arraySize(10){}

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

/**
 * Simulate newlists, add best list in the bestLists.
 */
template<typename Gametype>
void Selector<Gametype>::getBestBuildLists(vector<deque<string>>& newlists){
	//lastBuildLists = bestLists;
	for(auto list : newlists){
		try{
			Gametype g;
			g.readBuildList(list);
			g.simulate();

			Json::Value output = g.getOutput().getRawData();
			if(output["buildlistValid"] == 1 && output["messages"].size() > 0){

				int compareCriteria = getCompareCriteria(output);

				if(compareCriteria != INT_MAX)
					bestLists.push(make_pair(list,compareCriteria));
			}
		}catch(SimulationException){
			//ignore buildList if it's invalid
		}
	}

	while(bestLists.size() > arraySize)
		bestLists.pop();
}

//force creation of all possible and necessarry selector classes
#include "Game.h"
template class Selector<ProtossGame>;
template class Selector<TerranGame>;
template class Selector<ZergGame>;