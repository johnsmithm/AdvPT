#include "Creator.hpp"

#include <iostream>
#include <cstdlib>
#include <algorithm>

using namespace std;

void Creator::createNextGeneration(vector<list<string>> curGen, vector<list<string>>& nextGen){
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
void Creator::getDeeperDependencies(string target, vector<list<string>>& deeperDependencies){
	GameObject targetGO = GameObject::get(target);
	vector<string> dependencies = targetGO.getDependencyNames();

	if(dependencies.size() == 0){
		deeperDependencies.push_back({});
		return;
	}

	for(string dependency : dependencies){
		getDeeperDependencies(dependency, deeperDependencies);
		for(list<string>& dependencyTree : deeperDependencies){
			dependencyTree.push_back(dependency);
		}
	}
}

void Creator::mutate(vector<list<string>>& buildLists){
	for(list<string>& buildList : buildLists){
		//delete an item from the build list, if a random value is lower
		//than its deletion probability
		for(auto iterator=buildList.begin(); iterator != buildList.end();){
			int random = rand();

			if(random < GameObject::get(*iterator).getDeletionProbability()){
				buildList.erase(iterator++);
			}else{
				iterator++;
			}
		}

		//insert an item at a random position into the build list, if a random value is lower
		//than its addition probability
		for(GameObject *go : GameObject::getAll([=](GameObject &go){return go.getRace() == targetRace;})){
			int random = rand();
			if(random < go->getIntroductionProbability()){
				int insertAfter = buildList.size() != 0 ? rand()%buildList.size() : 0;

				auto positional=buildList.begin();
				for(int i=0; i<insertAfter; i++, positional++);

				buildList.insert(positional, go->getName());
			}
		}
	}

}

/**
 * Hamming distance.
 */
int Creator::getDistance (list<string>& a,list<string>& b) {
	int value = 0;
	auto ita = a.begin();
	auto itb = b.begin();
	for(; ita != a.end() && itb != b.end();)
		if(*ita != *itb)
			++value;

	return value;
}

bool Creator::checkValidity(list<string>& listL, string newOne){

	GameObject go = GameObject::get(newOne);
	vector<string> producers = go.getProducerNames();
	bool ok = false;
	for(auto name : producers)
		if(listL.end() != find(listL.begin(), listL.end(), name))
			ok = true;
	if(!ok)
		return false;

	vector<string> dependencies = go.getDependencyNames();
	ok = false;
	for(auto name : dependencies)
		if(listL.end() != find(listL.begin(), listL.end(), name))
			ok = true;
	if(!ok)return false;

	string gasMaker = "assimilator"; //Depends on race

	if(go.getGasCost() > 0 && listL.end() == find(listL.begin(), listL.end(), gasMaker))
		return false;

	if(supplyCheck - go.getSupplyCost() > 0)
		return false;

	supplyCheck += go.getSupplyProvided() - go.getSupplyCost();

	return true;

}


bool Creator::checkBuildLists(list<string> listL){
	supplyCheck = 0;
	list<string> newlist;
	for(auto item : listL){
		if(checkValidity(newlist, item))
			return false;
		newlist.push_back(item);
	}
	return true;
}



void Creator::getChild(list<string>& a, list<string>& b, list<string>& newList){

	supplyCheck = 0;
	int coin = rand() % 2;
	auto ita = a.begin();
	auto itb = b.begin();
	for(; ita != a.end() && itb != b.end();){
		if(coin && checkValidity(newList,*ita)){
				newList.push_back(*ita);
		}else{
			if(checkValidity(newList,*itb))
				newList.push_back(*itb);
			else
				newList.push_back(*ita);
		}
		coin = rand() % 2;
		++ita;
		++itb;
	}
}


/**
 * Combine parents base on Hamming distance. O(bestlist.size^2 * list.size * name.size)
 * Combine the different ones.
 */
void Creator::reproduce(vector<list<string>>& bestLists, vector<list<string>>& children){
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

		list<string> newChild;
		getChild(bestLists[i],bestLists[maxID],newChild);
		children.push_back(newChild);
	}

}

/**
 * @brief Exchange blocks of genes.
 * blocks are different size, exchange at same position
 * n + 1 - number of blocks
 */
vector<list<string>> nPointsCrossover(list<string> a,list<string> b, size_t n){
	int trys = 0;
	size_t maxL = max(a.size(), b.size()) - 1;
	n = min(maxL / 2, n);
	list<string> firstChild;
	list<string> secondChild;
	list<string> firstChildNew;
	list<string> secondChildNew;
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