#include "Creator.hpp"

#include <iostream>

using namespace std;

vector<vector<string>> Creator::createInitialBuildList(string target){
	vector<vector<string>> buildLists = getDeeperDependencies(target);

	for(vector<string>& buildList : buildLists){
		buildList.push_back(target);
	}

	//TODO: Producer of all entries of the build-list and their dependencies
	//have to be added too... But in which order? Maybe we should not generate
	//a buildList based on dependencies at all...

	return buildLists;
}

vector<vector<string>> Creator::getDeeperDependencies(string target){
	GameObject targetGO = GameObject::get(target);
	vector<string> dependencies = targetGO.getDependencyNames();

	if(dependencies.size() == 0){
		return {{}};
	}

	vector<vector<string>> deeperDependencies;
	for(string dependency : dependencies){
		for(vector<string>& dependencyTree : getDeeperDependencies(dependency)){
			dependencyTree.push_back(dependency);
			deeperDependencies.push_back(dependencyTree);
		}
	}

	return deeperDependencies;
}