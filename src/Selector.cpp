#include "Selector.hpp"

#include <chrono>
#include <iomanip>

#include "Diagnosis.hpp"

template<typename Gametype>
Selector<Gametype>::Selector(OptimizationMode setMode, string setTarget)
:mode(setMode), target(setTarget), arraySize(30), creator(setTarget, setMode){}


template<typename Gametype>
void Selector<Gametype>::optimize(int maxIterations){
	vector<list<string>> curGen;
	creator.createInitialBuildList(target, curGen);

	list<pair<list<string>, int>> bestLists;

	#ifdef DIAGNOSE
		cout << "Beginning optimization" << endl;
	#endif

	chrono::time_point<std::chrono::system_clock> start,end;
	start = std::chrono::high_resolution_clock::now();

	for(int i=0; i<maxIterations; i++){
		#ifdef DIAGNOSE_PRINT_STEPS
		if(!(i%10)){
			end = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> elapsed_seconds = end-start;
			cout << "Iteration " << setw(4) << setfill('0') << i << "/" << maxIterations;
			if(i!=0)
				cout << " with an average iteration time of " << elapsed_seconds.count()/i*1000 << "ms" << endl;

				if(bestLists.size() != 0){
					cout << "\t current best (score: " << bestLists.front().second << ") :";
					printBuildList(bestLists.front().first, cout);
				}
			else
				cout << endl;
		}
		#endif

		vector<list<string>> nextGen;
		#ifdef DIAGNOSE_NEXTGEN
			measureTime<void>(bind(&Creator::createNextGeneration, &creator, curGen, ref(nextGen)), "createNextGeneration");
		#else
			creator.createNextGeneration(curGen, nextGen);
		#endif

		curGen.insert(curGen.end(), nextGen.begin(), nextGen.end());

		#ifdef DIAGNOSE_GETBEST
			measureTime<void>(bind(&Selector<Gametype>::getBestBuildLists, this, ref(curGen), ref(bestLists)), "getBestBuildLists");
		#else
			getBestBuildLists(curGen, bestLists);
		#endif

		curGen.clear();
		for(auto list : bestLists)
			curGen.push_back(list.first);
	}

	if(bestLists.size() == 0){
		cout << "optimization failed, no results!" << endl;
		return;
	}

	#ifdef DIAGNOSE_PRINT_RESULT
		auto bestList = bestLists.front();
		for(auto s : bestList.first){
			cerr << s << "->";
		}
		cerr << bestList.first.size();
	#else
		Gametype g;
		g.readBuildList(bestLists.front().first);
		g.simulate();
		g.getOutput().print();
	#endif
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
			#ifdef DIAGNOSE_SIMULATE
				measureTime<void>(bind(&Gametype::simulate, &g), "simulate");
			#else
				g.simulate();
			#endif
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
