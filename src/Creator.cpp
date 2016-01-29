#include "Creator.hpp"

#include <iostream>
#include <cstdlib>
#include <algorithm>

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
		for(GameObject *go : GameObject::getAll([=](GameObject &go){return go.getRace() == targetRace;})){
			int random = rand();
			if(random < go->getIntroductionProbability()){
				int insertAfter = buildList.size() != 0 ? rand()%buildList.size() : 0;
				buildList.insert(buildList.begin()+insertAfter, go->getName());
			}
		}
	}

}

/**
 * Hamming distance.
 */
int Creator::getDistance (vector<string> a,vector<string> b) {
	int value = 0;
	for(size_t i=0; i < a.size() && i < b.size(); ++i)
		if(a[i] != b[i])
			++value;
	return value;
}

bool Creator::checkValidity(vector<string> list, string newOne){

	GameObject go = GameObject::get(newOne);
	vector<string> producers = go.getProducerNames();
	bool ok = false;
	for(auto name : producers)
		if(list.end() != find(list.begin(), list.end(), name))
			ok = true;
	if(!ok)return false;

	vector<string> dependencies = go.getDependencyNames();
	ok = false;
	for(auto name : dependencies)
		if(list.end() != find(list.begin(), list.end(), name))
			ok = true;
	if(!ok)return false;

	string gasMaker = "assimilator"; //Depends on race

	if(go.getGasCost() > 0 && list.end() == find(list.begin(), list.end(), gasMaker))
		return false;

	if(supplyCheck - go.getSupplyCost() > 0)
		return false;

	supplyCheck += go.getSupplyProvided() - go.getSupplyCost();

	return true;

}

bool checkBuildList(vector<string> list){
	supplyCheck = 0;
	vector<string> newlist;
	for(auto item : list){
		if(checkValidity(newlist, item))
			return false;
		newlist.push_back(item);
	}
	return true;
}

vector<string> Creator::getChild(vector<string> a,vector<string> b){
	supplyCheck = 0;
	int coin = rand() % 2;
	vector<string> newList;
	for(size_t i=0; i < a.size() && i < b.size(); ++i){
		if(coin && checkValidity(newList,a[i])){			
				newList.push_back(a[i]);					
		}else{
			if(checkValidity(newList,b[i]))
				newList.push_back(b[i]);
			else
				newList.push_back(a[i]);
		}
		coin = rand() % 2;
	}
	return newList;
}

/**
 * Combine parents base on Hamming distance. O(bestlist.size^2 * list.size * name.size)
 * Combine the different ones.
 */
vector<vector<string>> Creator::reproductionDistance(vector<vector<string>> bestLists){
	int maxD;
	int maxID;
	//int vizit[bestLists.size()];
	vector<vector<string>> children;

	for(size_t i=0; i<bestLists.size() - 1; ++i)
	{
		maxD = 0;
		maxID = i + 1;
		for(size_t j= i+1; j<bestLists.size(); ++j){
			int d = getDistance(bestLists[i], bestLists[j]);
			if(d > maxD){
				maxD = d;
				maxID = j;
			}
		}
		children.push_back(getChild(bestLists[i],bestLists[maxID]));
	}
	return children;
}

/**
 * @brief Exchange blocks of genes.
 * blocks are different size, exchange at same position
 * n + 1 - number of blocks
 */
vector<deque<string>> nPointsCrossover(deque<string> a,deque<string> b, int n){
	int trys = 0;
	size_t maxL = max(a.size(), b.size()) - 1;
	n = min(maxL / 2, n);
	deque<string> firstChild;
	deque<string> secondChild;
	deque<string> firstChildNew;
	deque<string> secondChildNew;
	bool ok1 = false;
	bool ok2 = false;
	do{
		firstChild = a;
		secondChild = b;
		firstChildNew.clear();
		secondChildNew.clear();
		for(int i=0; i<n; ++i){
			firstChildNew.clear();
			secondChildNew.clear();
			int position = rand() % maxL;

			firstChildNew.splice(firstChildNew.begin(),firstChild.begin(), firstChild.begin() + position);
			secondChildNew.splice(secondChildNew.begin(),secondChild.begin(), secondChild.begin() + position);

			firstChildNew.splice(firstChildNew.begin() + position ,secondChildNew.begin() + position, secondChildNew.end());
			secondChildNew.splice(secondChildNew.begin() + position,firstChild.begin() + position, firstChild.begin());
			
			firstChild = firstChildNew;
			secondChild = secondChildNew;
		}
		ok1 = checkBuildList(firstChild);
		ok2 = checkBuildList(secondChild);
		if(ok1 || ok2){
			if(ok1 && ok2){
				return {firstChild,secondChild};
			}else if (ok1) return {firstChild};
			else return {secondChild}
			break;
		}
	}while(trys < 3);
}