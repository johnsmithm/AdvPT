#include "Creator.hpp"

#include <iostream>
#include <cstdlib>
#include <algorithm>

using namespace std;

void Creator::createNextGeneration(vector<deque<string>> curGen, vector<deque<string>>& nextGen){
	if(nextGen.size() != 0 && curGen.size() != 0)
		reproduce(curGen, nextGen);

	mutate(curGen);

	nextGen.insert(nextGen.end(), curGen.begin(), curGen.end());
}

void Creator::createInitialBuildList(string target, vector<deque<string>>& buildLists){
	getDeeperDependencies(target, buildLists);

	for(deque<string>& buildList : buildLists){
		buildList.push_back(target);
	}

	//TODO: Producer of all entries of the build-list and their dependencies
	//have to be added too... But in which order? Maybe we should not generate
	//a buildList based on dependencies at all...
}

/** @brief returns recursively all possible dependency trees leading to target
 */
void Creator::getDeeperDependencies(string target, vector<deque<string>>& deeperDependencies){
	GameObject targetGO = GameObject::get(target);
	vector<string> dependencies = targetGO.getDependencyNames();

	if(dependencies.size() == 0){
		deeperDependencies.push_back({});
		return;
	}

	for(string dependency : dependencies){
		getDeeperDependencies(dependency, deeperDependencies);
		for(deque<string>& dependencyTree : deeperDependencies){
			dependencyTree.push_back(dependency);
		}
	}
}

void Creator::mutate(vector<deque<string>>& buildLists){
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
int Creator::getDistance (deque<string>& a,deque<string>& b) {
	int value = 0;
	for(size_t i=0; i < a.size() && i < b.size(); ++i)
		if(a[i] != b[i])
			++value;
	return value;
}

bool Creator::checkValidity(deque<string>& list, string newOne){

	GameObject go = GameObject::get(newOne);
	vector<string> producers = go.getProducerNames();
	bool ok = false;
	for(auto name : producers)
		if(list.end() != find(list.begin(), list.end(), name))
			ok = true;
	if(!ok)
		return false;

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


bool Creator::checkBuildList(deque<string> list){
	supplyCheck = 0;
	deque<string> newlist;
	for(auto item : list){
		if(checkValidity(newlist, item))
			return false;
		newlist.push_back(item);
	}
	return true;
}


void Creator::getChild(deque<string>& a, deque<string>& b, deque<string>& newList){

	supplyCheck = 0;
	int coin = rand() % 2;
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
}

/**
 * Combine parents base on Hamming distance. O(bestlist.size^2 * list.size * name.size)
 * Combine the different ones.
 */
void Creator::reproduce(vector<deque<string>>& bestLists, vector<deque<string>>& children){
	int maxD;
	int maxID;
	//int vizit[bestLists.size()];

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

		deque<string> newChild;
		getChild(bestLists[i],bestLists[maxID],newChild);
		children.push_back(newChild);
	}

}

/**
 * @brief Exchange blocks of genes.
 * blocks are different size, exchange at same position
 * n + 1 - number of blocks
 */
vector<deque<string>> nPointsCrossover(deque<string> a,deque<string> b, size_t n){
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
		for(size_t i=0; i<n; ++i){
			firstChildNew.clear();
			secondChildNew.clear();
			//size_t position = rand() % maxL;

			//firstChildNew.splice(firstChildNew.begin(),firstChild.begin(), firstChild.begin() + position);
			//secondChildNew.splice(secondChildNew.begin(),secondChild.begin(), secondChild.begin() + position);

			//firstChildNew.splice(firstChildNew.begin() + position ,secondChildNew.begin() + position, secondChildNew.end());
			//secondChildNew.splice(secondChildNew.begin() + position,firstChild.begin() + position, firstChild.begin());

			firstChild = firstChildNew;
			secondChild = secondChildNew;
		}
		//ok1 = checkBuildList(firstChild);
		//ok2 = checkBuildList(secondChild);
		if(ok1 || ok2){
			if(ok1 && ok2){
				return {firstChild,secondChild};
			}else if (ok1) return {firstChild};
			else return {secondChild};
			break;
		}
	}while(trys < 3);
	return {};
}