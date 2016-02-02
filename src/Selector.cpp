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
		if(!(i%100)){
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
		if (GameObject::get(target).getInstancesCount(game.getInstances()) == 0)
		    score += 1000;
		return score;
	} else {
		if(game.getCurrentTime() - 1 <= 360)
			return (- GameObject::get(target).getInstancesCount(game.getInstances()));
		return INT_MAX;
	}
}

/**
 * Simulate newlists, add best list in the bestLists.
 */
template<typename Gametype>
void Selector<Gametype>::getBestBuildLists(vector<list<string>>& newlists, list<pair<list<string>, int>>& bestLists){
	const int threadnum = 8;

	thread threads[threadnum];
	if(bestLists.size() > threadnum && 0){
		for(int i=0; i<threadnum; i++){
			threads[i] = thread(&Selector<Gametype>::threaded_evalution, this, ref(newlists), i, threadnum, ref(bestLists));
		}
		for(int i=0; i<threadnum; i++){
			threads[i].join();
		}
	}else{
		threaded_evalution(newlists, 0, 1, bestLists);
	}

	bestLists.sort(ListComparator());

	while(bestLists.size() > arraySize)
		bestLists.pop_back();
}

template<typename Gametype>
void Selector<Gametype>::threaded_evalution(vector<list<string>>& newlists, size_t startIndex, size_t stepsize, list<pair<list<string>, int>>& bestLists){
	assert(newlists.size()>startIndex);
	assert(stepsize >= startIndex);

	int compareCriteria;
	vector<list<string>>::iterator iterator = newlists.begin();
	for(size_t i=0; i<startIndex; i++){
		iterator++;
	}

	list<pair<list<string>, int>> localBestLists;

	while(true){
		auto list = *iterator;

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
		}catch(SimulationException e){
			// cerr << e.what() << endl;
			compareCriteria = 2000;
		}
		localBestLists.push_back(make_pair(list,compareCriteria));

		//go through the lists with stepsize-sized steps
		for(size_t i=0; i<stepsize; i++){
			iterator++;

			//we've reached the end => copy back our results and return
			if(iterator == newlists.end()){
				lock_guard<mutex> lock(evaluation_mutex);
				bestLists.insert(bestLists.end(), localBestLists.begin(), localBestLists.end());
				return;
			}
		}
	}

}

//force creation of all possible and necessarry selector classes
#include "Game.h"
template class Selector<ProtossGame>;
template class Selector<TerranGame>;
template class Selector<ZergGame>;
