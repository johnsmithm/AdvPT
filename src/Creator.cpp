#include "Creator.hpp"

#include <iostream>
#include <cstdlib>

using namespace std;

vector<deque<string>> Creator::createInitialBuildList(string target){
	vector<deque<string>> buildLists = getDeeperDependencies(target);

	for(deque<string>& buildList : buildLists){
		buildList.push_back(target);
	}

	//TODO: Producer of all entries of the build-list and their dependencies
	//have to be added too... But in which order? Maybe we should not generate
	//a buildList based on dependencies at all...

	return buildLists;
}

vector<deque<string>> Creator::getDeeperDependencies(string target){
	GameObject targetGO = GameObject::get(target);
	vector<string> dependencies = targetGO.getDependencyNames();

	if(dependencies.size() == 0){
		return {{}};
	}

	vector<deque<string>> deeperDependencies;
	for(string dependency : dependencies){
		for(deque<string>& dependencyTree : getDeeperDependencies(dependency)){
			dependencyTree.push_back(dependency);
			deeperDependencies.push_back(dependencyTree);
		}
	}

	return deeperDependencies;
}

void Creator::mutateBuildLists(vector<deque<string>>& buildLists){
	for(deque<string>& buildList : buildLists){
		//delete an item from the build list, if a random value is lower
		//than its deletion probability
		for(unsigned int i = 0; i < buildList.size(); i++){
			int random = rand();

			if(random < GameObject::get(buildList[i]).getDeletionProbability()){
				buildList.erase(buildList.begin()+i);
				i--;
			}
		}

		//delete an item at a random position into the build list, if a random value is lower
		//than its addition probability
		for(GameObject *go : GameObject::getAll()){
			int random = rand();
			if(random < go->getIntroductionProbability()){
				int insertAfter = buildList.size() != 0 ? rand()%buildList.size() : 0;
				buildList.insert(buildList.begin()+insertAfter, go->getName());
			}
		}
	}

}