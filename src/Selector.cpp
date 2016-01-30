#include "Selector.hpp"

#include <chrono>
#include <iomanip>

/** @brief reads a buildList from any iterable container of strings
  */
template<template <typename, typename> class Container>
void debugPrint(Container<std::string, std::allocator<std::string>> &input, ostream& stream=cerr){
    for(std::string line : input){
        stream << line << "->";
    }
    stream << input.size() << endl;
}


template<typename Gametype>
Selector<Gametype>::Selector(OptimizationMode setMode, string setTarget)
:mode(setMode), target(setTarget), arraySize(500), creator(setTarget, setMode){}


template<typename Gametype>
void Selector<Gametype>::optimize(int maxIterations){
	vector<list<string>> curGen;
	creator.createInitialBuildList(target, curGen);

	list<pair<list<string>, int>> bestLists;

	cout << "Beginning optimization" << endl;

	chrono::time_point<std::chrono::system_clock> start,end;
	start = std::chrono::high_resolution_clock::now();

	for(int i=0; i<maxIterations; i++){
		if(!(i%10)){
			end = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> elapsed_seconds = end-start;
			cout << "Iteration " << setw(4) << setfill('0') << i << "/" << maxIterations;
			if(i!=0)
				cout << " with an average iteration time of " << elapsed_seconds.count()/i*1000 << "ms" << endl;

				if(bestLists.size() != 0){
					cout << "\t current best (score: " << bestLists.front().second << ") :";
					debugPrint(bestLists.front().first, cout);
				}
			else
				cout << endl;
		}

		vector<list<string>> nextGen;
		creator.createNextGeneration(curGen, nextGen);

		curGen.insert(curGen.end(), nextGen.begin(), nextGen.end());

		getBestBuildLists(curGen, bestLists);

		curGen.clear();
		for(auto list : bestLists)
			curGen.push_back(list.first);
	}

	if(bestLists.size() == 0){
		cout << "optimization failed, no results!" << endl;
		return;
	}

	auto bestList = bestLists.front();
	for(auto s : bestList.first){
		cout << s << "->";
	}
	cout << bestList.first.size();
}

/** @brief returns the fitness of a buildList from the output of its simulation
 *
 *  The smaller the output the fitter the buildList. Invalid build-lists return INT_MAX
 */
template<typename Gametype>
int Selector<Gametype>::getCompareCriteria(Game& game){
	if(mode == OptimizationMode::PUSH) {
		int score = game.getCurrentTime();
		if (GameObject::get(target).getInstancesCount() == 0)
		    score += 1000;
		return score;
	} else {
		/*int count = 0;

		if(output["messages"][output["messages"].size()-1]["time"].asInt() > 360)
			return INT_MAX;

		for(auto message : output["messages"])
			for(auto event : message["events"])
				if(event["type"] == "build-end" && event["name"] == target)
					++count;

		return -count;*/
		return 0;
	}
}

/**
 * Simulate newlists, add best list in the bestLists.
 */
template<typename Gametype>
void Selector<Gametype>::getBestBuildLists(vector<list<string>>& newlists, list<pair<list<string>, int>>& bestLists){
	//lastBuildLists = bestLists;
	int compareCriteria;
	for(auto list : newlists){
		if(list.size() ==0)
			continue;
		try{
			Gametype g;
			g.readBuildList(list);
			g.simulate();
			compareCriteria = getCompareCriteria(g);
			GameObject::removeAllInstances();
		}catch(SimulationException){
			compareCriteria = 2000;
		}
		bestLists.push_back(make_pair(list,compareCriteria));
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
