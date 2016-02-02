#include "Creator.hpp"

#include <iostream>
#include <algorithm>
#include <list>

using namespace std;


Creator::Creator(string targetUnit, OptimizationMode modeC)
: targetRace(GameObject::get(targetUnit).getRace()), targetUnit(targetUnit), modeC(modeC){
	randgen.seed(45038961);
	if(targetRace == Race::PROTOSS){
		baseworker = "probe";
		baseBuilding = "nexus";
		gasMaker = "assimilator";
		supplyBuilding = "pylon";
	}else if(targetRace == Race::TERRAN){
		baseworker = "scv";
		baseBuilding = "command_center";
		gasMaker = "refinery";
		supplyBuilding = "supply_depot";
	}else{
		baseworker = "drone";
		baseBuilding = "hatchery";
		gasMaker = "extractor";
		supplyBuilding = "overlord";
	}

	involvedUnits.push_back(baseBuilding);
	involvedUnits.push_back(baseworker);
	involvedUnits.push_back(supplyBuilding);
	if(gasNeeded(targetUnit))
		involvedUnits.push_back(gasMaker);



	if(modeC == OptimizationMode::RUSH){
		for(auto name : involvedUnits){
			GameObject::get(name).setIntroductionProbability(10);
			GameObject::get(name).setDeletionProbability(9);
		}

		GameObject::get(baseworker).setIntroductionProbability(40);
		GameObject::get(baseBuilding).setIntroductionProbability(10);
		GameObject::get(baseworker).setDeletionProbability(40);
		GameObject::get(baseBuilding).setDeletionProbability(10);

		GameObject::get(targetUnit).setIntroductionProbability(40);
		GameObject::get(targetUnit).setDeletionProbability(10);

		vector<string> targetProducer = GameObject::get(targetUnit).getProducerNames();
		for(auto name : targetProducer){
			GameObject::get(name).setIntroductionProbability(20);
			GameObject::get(name).setDeletionProbability(15);
		}
	}else{
		GameObject::get(baseworker).setIntroductionProbability(40);
		GameObject::get(baseBuilding).setIntroductionProbability(10);
		GameObject::get(baseworker).setDeletionProbability(40);
		GameObject::get(baseBuilding).setDeletionProbability(10);
	}
}

/**
 * get all involved units
 */
bool Creator::gasNeeded(string name){
	if(involvedUnits.end() != find(involvedUnits.begin(), involvedUnits.end(), name) || name == "larva")
		return false;

	involvedUnits.push_back(name);

	GameObject& targetGO = GameObject::get(name);
	bool needGas = targetGO.getSupplyCost() > 0;

	vector<string> producerNames = targetGO.getProducerNames();
	for(size_t i = 0; i < producerNames.size(); ++i){
		needGas = gasNeeded(producerNames[i]) || needGas;
	}

	vector<string> dependencyNames = targetGO.getDependencyNames();
	for(auto item : dependencyNames){
		needGas = gasNeeded(item) || needGas;
		break;
	}
	return needGas;
}

void Creator::createNextGeneration(vector<list<string>> curGen, vector<list<string>>& nextGen){
	if(curGen.size() != 0)
		reproduce(curGen, nextGen);

    vector<list<string>> curGen1 = curGen;
    mutate(curGen1);
    nextGen.insert(nextGen.end(), curGen1.begin(), curGen1.end());

    switchGenesMutation(curGen,5,1);
	nextGen.insert(nextGen.end(), curGen.begin(), curGen.end());
}

void Creator::addDrone(vector<list<string>>& buildLists){
	for(list<string> & buildList : buildLists){
		int count = 0;
		for(auto item : buildList)
			if(GameObject::get(item).getProducerNames()[0] == "drone")
				++count;
		count*=2;
		buildList.push_front(gasMaker);
		auto it = buildList.begin();
		while(count>=0 && it != buildList.end()){
			if(*it == "spawning_pool"){
				buildList.insert(it, "drone");
				++it;
				buildList.insert(it, "queen");
			}else{
				buildList.insert(it, "drone");
				++it;
			}
			--count;
		}
	}
}

void Creator::createInitialBuildList(string target, vector<list<string>>& buildLists){
	vector<string> viz;
	buildLists.push_back({});
	getDeeperDependencies(target, buildLists,viz,{}, 0);

	for(list<string>& buildList : buildLists){
		//buildList.push_back(target);
		buildList.push_front(gasMaker);
		buildList.push_front(supplyBuilding);
	}

	if(targetRace == Race::ZERG){
		addDrone(buildLists);
	}

	//TODO: Producer of all entries of the build-list and their dependencies
	//have to be added too... But in which order? Maybe we should not generate
	//a buildList based on dependencies at all...
}

/** @brief returns recursively all possible dependency trees leading to target
 */
void Creator::getDeeperDependencies(string target, vector<list<string>>& deeperDependencies,vector<string>& vizitedA, vector<string> vizited, size_t idList){
	if(target == "larva" || target == baseworker || target == baseBuilding
		|| vizitedA.end() != find(vizitedA.begin(), vizitedA.end(), target))
       return;

    vizitedA.push_back(target);
    vizited.push_back(target);

	GameObject& targetGO = GameObject::get(target);

	vector<string> producerNames = targetGO.getProducerNames();
	for(size_t i = 0; i < producerNames.size(); ++i){
        if(i == producerNames.size() - 1)
       		getDeeperDependencies(producerNames[i], deeperDependencies,vizitedA, vizited, idList);
       	else{
       		list<string> v = deeperDependencies[idList];
       		deeperDependencies.push_back(v);
       		vector<string> viz = vizited;
       		getDeeperDependencies(producerNames[i], deeperDependencies,viz, vizited, idList + 1 + i);
       	}
    }

	vector<string> dependencyNames = targetGO.getDependencyNames();
	for(auto item : dependencyNames){
       	getDeeperDependencies(item, deeperDependencies,vizitedA, vizited, idList);
        break;
    }

    for(size_t i = 0; i < producerNames.size(); ++i)
    	deeperDependencies[idList + i].push_back(target);
}

void Creator::mutate(vector<list<string>>& buildLists){
    std::uniform_int_distribution<std::default_random_engine::result_type> rand(0,99);

	for(list<string>& buildList : buildLists){
		//delete an item from the build list, if a random value is lower
		//than its deletion probability
		for(auto iterator=buildList.begin(); iterator != buildList.end();){
			int random = rand(randgen);

			if(random < GameObject::get(*iterator).getDeletionProbability()){
				buildList.erase(iterator++);
			}else{
				iterator++;
			}
		}

		//insert an item at a random position into the build list, if a random value is lower
		//than its addition probability
		//for(GameObject *go : /*GameObject::getAll([=](GameObject &go){return go.getRace() == targetRace;})*/){
		for(auto name : involvedUnits){
			auto go = GameObject::get(name);
			int random = rand(randgen);
			if(random < go.getIntroductionProbability()){
				int insertAfter = buildList.size() != 0 ? rand(randgen)%buildList.size() : 0;

				auto positional=buildList.begin();
				for(int i=0; i<insertAfter; i++, positional++);

				buildList.insert(positional, name);
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
	for(; ita != a.end() && itb != b.end();++itb,++ita)
		if(*ita != *itb)
			++value;

	return value;
}

//checked
bool Creator::checkValidity(list<string>& listL, string newOne){

	GameObject go = GameObject::get(newOne);
	vector<string> producers = go.getProducerNames();
	bool ok = producers.size() == 0;
	for(auto name : producers)
		if(listL.end() != find(listL.begin(), listL.end(), name) || name == baseworker || name == baseBuilding)
			ok = true;
	if(!ok)
		return false;

	vector<string> dependencies = go.getDependencyNames();
	ok = dependencies.size() == 0;
	for(auto name : dependencies)
		if(listL.end() != find(listL.begin(), listL.end(), name))
			ok = true;
	if(!ok)return false;

	if(go.getGasCost() > 0 && listL.end() == find(listL.begin(), listL.end(), gasMaker))
		return false;

	if((supplyCheck < (int)go.getSupplyCost()))
		return false;

	supplyCheck += go.getSupplyProvided() - go.getSupplyCost();

	return true;

}

//checked
bool Creator::checkBuildLists(list<string> listL){
	supplyCheck = GameObject::get(baseBuilding).getSupplyProvided() - 6*GameObject::get(baseworker).getSupplyCost();

	list<string> newlist;
	for(auto item : listL){
		if(!checkValidity(newlist, item))
			return false;
		newlist.push_back(item);
	}

	return true;
}
//checked
bool Creator::meetGoal(list<string> newList, list<string> oldList){
	if(newList.end() == find(newList.begin(), newList.end(), targetUnit))
			return false;
	if(modeC == OptimizationMode::RUSH){
		//count #targets
	}
	return true;
}
//checked
void Creator::getChild(list<string>& a, list<string>& b, list<string>& newList){
    std::uniform_int_distribution<std::default_random_engine::result_type> rand(0,1);

	supplyCheck = GameObject::get(baseBuilding).getSupplyProvided() - 6*GameObject::get(baseworker).getSupplyCost();
	int coin = rand(randgen);
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
		//maybe we should switch if we choose too many times one side
		coin = rand(randgen);
		++ita;
		++itb;
	}
	if(!meetGoal(newList,{})){
		if(ita != a.end()){
			newList.insert(newList.end(),ita, a.end());
		}else newList.insert(newList.end(),itb, b.end());
	}
}

//checked
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
		//maybe should check if we use that buildlist ones
		getChild(bestLists[i],bestLists[maxID],newChild);
		children.push_back(newChild);

		list<string> newChild1 = nLengthCrossover(bestLists[i],bestLists[maxID],2);
		children.push_back(newChild1);
	}

	for(size_t i=0; i<bestLists.size(); ++i){
		std::uniform_int_distribution<std::default_random_engine::result_type> rand(0,bestLists.size()-1);
		size_t position = rand(randgen);
		if(i != position){

			vector<list<string>> newList = nPointsCrossover(bestLists[i],bestLists[position],2);
			for(auto li : newList)
				children.push_back(li);

			list<string> newChild;
			getChild(bestLists[i],bestLists[position],newChild);
			children.push_back(newChild);

			list<string> newChild1 = nLengthCrossover(bestLists[i],bestLists[position],2);
			children.push_back(newChild1);
		}
	}
}

//checked
/**
 * @brief Exchange blocks of genes.
 * blocks are different size, exchange at same position
 * n + 1 - number of blocks
 */
vector<list<string>> Creator::nPointsCrossover(list<string> a,list<string> b, size_t n){
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

            std::uniform_int_distribution<std::default_random_engine::result_type> rand(0,maxL-1);
			size_t position = rand(randgen);

			auto vi1 = firstChild.begin();
			advance(vi1, position);
			firstChildNew.splice(firstChildNew.begin(),firstChild ,firstChild.begin(),  vi1);

			auto vi2 = secondChild.begin();
			advance(vi2, position);
			secondChildNew.splice(secondChildNew.begin(),secondChild, secondChild.begin(), vi2);

			auto vi3 = firstChildNew.begin();
			advance(vi3, position);
			firstChildNew.splice(vi3 ,secondChild, vi2, secondChild.end());

			auto vi4 = secondChildNew.begin();
			advance(vi4, position);
			secondChildNew.splice(vi4,firstChild, vi1, firstChild.end());

			firstChild = firstChildNew;
			secondChild = secondChildNew;
		}
		ok1 = checkBuildLists(firstChild);//why checkBuildLists is not seen?
		ok2 = checkBuildLists(secondChild);
		if(ok1 || ok2 || 1){
			if(ok1 || ok2 || 1){
				return {firstChild,secondChild};
			}else if (ok1) return {firstChild};
			else return {secondChild};
			break;
		}
	}while(++trys < 3);
	return {};
}

//checked
/**
 * @brief Blocks of length n are interchanged between lists a and b.
 */
list<string> Creator::nLengthCrossover(list<string> a,list<string> b, int n){
    std::uniform_int_distribution<std::default_random_engine::result_type> rand(0,1);
	supplyCheck = 0;
	int trys = 0;
	list<string> newList;
	while(trys++ < 3){
		int coin = rand(randgen);
		//auto ita = a.begin();
		auto itb = b.begin();
		newList = a;
		auto itN = newList.begin();
		int j = 0;
		for(; itb != b.end() && itN != newList.end();){
			if(coin){
				for(int i = 0;i< n && itb!=b.end() && itN != newList.end();++i, ++itb, ++itN)
					*itN = *itb;
			}else{
				advance(itN, n);				
				advance(itb, n);
			}
			coin = rand(randgen);
			++j;
		}
		if(!meetGoal(newList,{})){
			if(itb != b.end()){
				newList.insert(newList.end(),itb, b.end());
			}
		}
        if(checkBuildLists(newList))
			return newList;
	}
	return newList;
}

//checked
/**
 * @brief Creator::switchGenesMutation inverse times blocks of entries
 * @param lists
 * @param times numer of inversed blocks
 * @param length of the blocks
 */
void Creator::switchGenesMutation(vector<list<string>> &lists, int times, int length){
    for(int i=0; i<times;++i){
        for(list<string> & list : lists){
            //list<string> currentList = list;
            std::uniform_int_distribution<std::default_random_engine::result_type> rand(0,list.size()-1);
            int position = rand(randgen);

            std::list <string>::iterator itB = list.begin();
            std::list <string>::iterator itE = list.begin();

            for(int j=0;j<position;++j)++itB,++itE;
            for(int j=0;j<length && itE != list.end();++j)++itE;
            if(itE != list.end())++itE;
            string ex;
            while(itB != itE){
                --itE;
                if(itB == itE)break;
                ex = *itB;
                *itB = *itE;
                *itE = ex;
                ++itB;
            }
            //if(checkBuildLists(list))
               // list = currentList;
        }
    }
}
