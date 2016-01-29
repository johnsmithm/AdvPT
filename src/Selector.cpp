#include "Selector.hpp"


template<typename Gametype>
Selector<Gametype>::Selector(OptimizationMode setMode, string setTarget)
:mode(setMode), target(setTarget), arraySize(500), creator(GameObject::get(setTarget).getRace()){}


template<typename Gametype>
void Selector<Gametype>::optimize(int maxIterations){
	vector<deque<string>> curGen;
	creator.createInitialBuildList(target, curGen);

	list<pair<deque<string>, int>> bestLists;

	for(int i=0; i<maxIterations; i++){

		vector<deque<string>> nextGen;
		creator.createNextGeneration(curGen, nextGen);

		curGen.insert(curGen.end(), nextGen.begin(), nextGen.end());

		getBestBuildLists(curGen, bestLists);

		curGen.clear();
		for(auto list : bestLists)
			curGen.push_back(list.first);
	}
}

/** @brief returns the fitness of a buildList from the output of its simulation
 *
 *  The smaller the output the fitter the buildList. Invalid build-lists return INT_MAX
 */
template<typename Gametype>
int Selector<Gametype>::getCompareCriteria(Json::Value output){
	if(output["buildlistValid"] != 1 || output["messages"].size() == 0){
		return INT_MAX;
	}

	if(mode == OptimizationMode::PUSH) {
		return output["messages"][output["messages"].size()-1]["time"].asInt();
	} else {
		int count = 0;

		if(output["messages"][output["messages"].size()-1]["time"].asInt() > 360)
			return INT_MAX;

		for(auto message : output["messages"])
			for(auto event : message["events"])
				if(event["type"] == "build-end" && event["name"] == target)
					++count;

		return -count;
	}
}

/**
 * Simulate newlists, add best list in the bestLists.
 */
template<typename Gametype>
void Selector<Gametype>::getBestBuildLists(vector<deque<string>>& newlists, list<pair<deque<string>, int>>& bestLists){
	//lastBuildLists = bestLists;
	for(auto list : newlists){
		if(list.size() ==0)
			continue;
		try{
			Gametype g;
			g.readBuildList(list);
			g.simulate();
			GameObject::removeAllInstances();

			Json::Value output = g.getOutput().getRawData();
			if(output["buildlistValid"] == 1 && output["messages"].size() > 0){

				int compareCriteria = getCompareCriteria(output);

				if(compareCriteria != INT_MAX){
					bestLists.push_back(make_pair(list,compareCriteria));
				}
			}
		}catch(SimulationException){
			//ignore buildList if it's invalid
		}
	}

	bestLists.sort(ListComparator());

	while(bestLists.size() > arraySize)
		bestLists.pop_back();
}

//force creation of all possible and necessarry selector classes
#include "Game.h"
template class Selector<ProtossGame>;
template class Selector<TerranGame>;
template class Selector<ZergGame>;