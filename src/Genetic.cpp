#include "Genetic.h"
#include "util.h"
#include <string> 
#include <iostream>
#include <fstream>
#include <algorithm>

#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */


using namespace std;

Genetic::Genetic(std::string setRace, std::string setTypeSearch, std::string setUnit)
:race(setRace),typeSearch(setTypeSearch),unit(GameObject::get(setUnit))
{
  if(race == "protoss"){
    baseworker = "probe";
    baseBuilding = "nexus";
    gasMaker = "assimilator";
    supplyBuilding = "pylon";
  }else if(race == "terran"){
    baseworker = "scv";
    baseBuilding = "command_center";    
    gasMaker = "refinery";
    supplyBuilding = "supply_depot";
  }
}

/**
 * 
 */
void Genetic::findBuildList(){
  getNeededUnits();
  makeBuildList();//or makeBuildList1()
  do{
    selection();
    mutation();
    reproduction();
  }while(0);
}

void Genetic::printBest(){
  if(bestBuildListsJson.size()>0){
  	ofstream out("output.txt");
    //cout<<bestBuildListsJson[0];
    out<<bestBuildListsJson[0]<<"\n";

    vector<Json::Value> bTime;
    vector<Json::Value> minerals;
    vector<Json::Value> gas;

    if(bestBuildListsJson[0]["messages"].size() > 0)
    	for(auto item : bestBuildListsJson[0]["messages"])
    		if(item["events"].size()>0)
    			for(auto ev : item["events"])
    				if(ev["type"] == "build-start"){
    					bTime.push_back(item["time"]);
    					minerals.push_back(item["status"]["resources"]["minerals"]);
    					gas.push_back(item["status"]["resources"]["vespene"]);
    				}

    cout<<"name|beginTime|buildTime|mine|gas\n";
    for(size_t i=0; i<bestBuildListsString[0].size(); ++i)
    	if(bTime.size() > i)
    	cout<<bestBuildListsString[0][i]<<" "<<bTime[i]<<" "<<(GameObject::get(bestBuildListsString[0][i]).getBuildTime())<<" "<<minerals[i]<<" "<<gas[i]<<"\n";
    cout<<"Time:"<<bestBuildListsJson[0]["time"]<<"\n";
    //cout<<bestBuildListsString.size()<<"\n";
  }
}

bool Genetic::exist(vector<string> v,string name){
	if(v.end() != find(v.begin(),v.end(), name)){
     	return true;
     }
    return false; 
}

/**
 * Create the list of dependencies for the building.
 * Create 2^(list_lenght) list by adding the gas maker building
 */
void Genetic::getDependency(string name){
     if(exist(involvedUnits,name) || name == baseworker || name == baseBuilding)
       return;     

     involvedUnits.push_back(name);
     GameObject& go = GameObject::get(name);
     
     for(auto item : go.getProducerNames()){
       	getDependency(item);
        break;
     }

     for(auto item : go.getDependencyNames()){
       	getDependency(item);
        break;
     }

    vector<vector<string>> v;
    v = requiredOrder;

    for(size_t i=0; i<v.size(); ++i)
      v[i].push_back(gasMaker);

    static int add = 0;
    
    if(++add < 4)cout<<add;
    requiredOrder.insert(requiredOrder.begin(),v.begin(),v.end());

    for(size_t i=0; i<requiredOrder.size(); ++i){
       requiredOrder[i].push_back(name);
    }   
}

/**
 * Get time for harvesting minerals and gas with 3workers.
 */
size_t Genetic::getTime(int mineral, int gas, int workers){
  //ternarySearch(int left, int right, int neededGas,int neededMineral, int freeWorkers)
  int gasWorker = Game::ternarySearch(0,workers,gas,mineral,workers);
  //getMiningTime(int gasMiningWorkers, int mineralMiningWorkers, int neededGas,int neededMineral);
  return Game::getMiningTime(gasWorker, workers - gasWorker, gas, mineral);
  //how many gasWorker??
}

/**
 * For each unit of buildList get the number of workers needed 
 * to harvest the resources for the buildings, till the one that has 
 * dependency the current building.
 */
void Genetic::numberNeededWorker(){
   for(auto item : requiredOrder){
    vector<int> v;
    int neededWorkers = 6;
    for(size_t i = 0; i < item.size(); ++i){
      size_t k = i + 1;
      int gas = 0;
      int mineral = 0;
      vector<string> d,p ;      
      std::vector<string>::iterator dit;//equal with end when do not find something
      std::vector<string>::iterator pit;
      if(k < item.size()){
        d = GameObject::get(item[k]).getDependencyNames();   
        p = GameObject::get(item[k]).getProducerNames();
        dit = d.end();
        pit = p.end();
        while(k < item.size() && dit == d.end() && pit == p.end()){
          d = GameObject::get(item[k]).getDependencyNames();   
          p = GameObject::get(item[k]).getProducerNames();         
          gas +=  GameObject::get(item[k]).getGasCost();
          mineral += GameObject::get(item[k]).getMineralCost();
          dit = find(d.begin(),d.end(),item[i]);
          pit = find(p.begin(),p.end(),item[i]);
          ++k;
        }
      }
      if(item[i] != gasMaker && (k < item.size() || dit != d.end() || pit != p.end())){
          //getworkers ternarySearch
          while(GameObject::get(item[i]).getBuildTime() < getTime(mineral,gas,neededWorkers) && neededWorkers < 30)
            ++neededWorkers;
          v.push_back(neededWorkers);
      }else v.push_back(0);
    }
    requiredOrderWorkersNeeded.push_back(v);
    //need also max for a list => see last element
   }
}

/**
 * Make the first buildlist using the dependencies of the needed unit
 * Add gayserBuilding where is possible.
 */
void Genetic::getNeededUnits(){
  std::vector<string> v;
  requiredOrder.push_back(v);
  getDependency(unit.getName());

  //requiredOrder.erase(requiredOrder.begin());
  numberNeededWorker();

  resourcesUnits.push_back(baseworker);
  resourcesUnits.push_back(supplyBuilding);
  resourcesUnits.push_back(gasMaker);
  resourcesUnits.push_back(baseBuilding);
  int i = 0, j = 0;
  for(auto item1 : requiredOrder){
    j=0;
  	for(auto item : item1) {
      cout<<item<<" "<<requiredOrderWorkersNeeded[i][j]<<"\n";
      ++j;
    }
    ++i;
  	cout<<"\n";
  }
}

/**
 * Using rand to insert the worker in buildlist.
 */
void Genetic::makeBuildList(){
  //int supply = 4;
  srand (time(NULL));

  allBuildLists.clear();
  for(auto item : requiredOrder)
	  for (int i = 0; i < 10 ; ++ i){
	  	std::vector<std::string> mylist = item;
	  	for (int j=0 ; j <= i; ++j){
	  		int pos = rand() % (mylist.size()-1) ;
	  		mylist.insert(mylist.begin()+pos, baseworker);
	  	} 
	    allBuildLists.push_back(mylist);
	    mylist.clear();
	  } 
	//  allBuildLists.clear();
//allBuildLists.push_back({baseworker,baseworker,baseworker,gasMaker,baseworker,supplyBuilding,baseworker,gasMaker,"gateway",gasMaker,baseworker,"cybernetics_core","robotics_facility","robotics_bay","colossus"});	
	}

/**
 * Using info to inser the workers.(work bad)
 */
void Genetic::makeBuildList1(){
  int supply = 4;
  int workers = 6;
  int pylon = 0;
  int j = 0;
  srand (time(NULL));
  allBuildLists.clear();  
  std::vector<string> mylist;
  for(auto item : requiredOrder) {//each list
    workers = 6;
    pylon = 0;
    supply = 4;

    int k = item.size() - 1;
    while(requiredOrderWorkersNeeded[j][k] == 0 && k > 0)--k;
    int maxWorkers = 14;//requiredOrderWorkersNeeded[j][k];
   // cout<<k<<"lll\n";
  	  for (size_t i = 0; i < item.size() ; ++ i){

        if(item[i] == supplyBuilding && pylon == 1)continue;

  	  	int coin = (i > 0?GameObject::get(item[i-1]).getBuildTime():18) - 17;//rand() % 2 ;
        int p = 0;//need formula for adding workers
        while((coin > 0 && workers < maxWorkers && ((p < 2 && i<5) || (p<1 && i>3 && 0))) /*|| workers < requiredOrderWorkersNeeded[j][i]*/){
          if(supply == 0){
            mylist.push_back( supplyBuilding);
            pylon = 1;
            supply += 8;
            coin -= 25;
          }
          mylist.push_back( baseworker);
          supply--;
          workers++;
          coin -= 17;
          ++p;
        }     
          if(supply - GameObject::get(item[i]).getSupplyCost() < 0) {
            mylist.push_back( supplyBuilding);
            supply += 8;
          }
          mylist.push_back(item[i]);
          supply -= GameObject::get(item[i]).getSupplyCost();
          supply += GameObject::get(item[i]).getSupplyProvided();
  	  }
  	  allBuildLists.push_back(mylist);
  	  mylist.clear();
      ++j;
   }
   for(auto item : allBuildLists)
   {
    for(auto u : item)
      cout<<u<<"\n";
    cout<<"\n";
   }
}

/**
 * Simulate buildLists and select the best ones.
 */
void Genetic::selection(){
  if(race == "protoss"){
  	 for(auto item : allBuildLists){
  	 	Game *g;
  	 	g = new ProtossGame();
  	 	g->readBuildList(item);
  	 	g->simulate();
  	 	Json::Value output = g->getOutput().getJson();
  	 	g->deleteInstanaces();
      if(output["messages"].size() > 0) {//why do you have empty messages but have time???
    	 	if(bestBuildListsJson.size() > 4){
           if(bestBuildListsJson[bestBuildListsJson.size()-1]["time"] > output["time"]){     

              bestBuildListsJson.erase(bestBuildListsJson.begin() + 4);
              bestBuildListsString.erase(bestBuildListsString.begin() + 4);  

              int po = bestBuildListsString.size()-1;
              while(po > 0 && bestBuildListsJson[po]["time"] > output["time"])
                --po;

        	 		bestBuildListsJson.insert(bestBuildListsJson.begin() + po, output);
        	 		bestBuildListsString.insert(bestBuildListsString.begin() + po, item);
           } 
    	 	}else{
    	 		bestBuildListsJson.push_back(output);
    	 		bestBuildListsString.push_back(item);
    	 	}
      }
  	 	delete g;
  	 }
  }else if(race == "terran"){
    for(auto item : allBuildLists){
      Game *g;
      g = new TerranGame();
      g->readBuildList(item);
      g->simulate();
      Json::Value output = g->getOutput().getJson();
      g->deleteInstanaces();
      if(output["messages"].size() > 0) {//why do you have empty messages but have time???
        if(bestBuildListsJson.size() > 4){
           if(bestBuildListsJson[bestBuildListsJson.size()-1]["time"] > output["time"]){     

              bestBuildListsJson.erase(bestBuildListsJson.begin() + 4);
              bestBuildListsString.erase(bestBuildListsString.begin() + 4);  

              int po = bestBuildListsString.size()-1;
              while(po > 0 && bestBuildListsJson[po]["time"] > output["time"])
                --po;

              bestBuildListsJson.insert(bestBuildListsJson.begin() + po, output);
              bestBuildListsString.insert(bestBuildListsString.begin() + po, item);
           } 
        }else{
          bestBuildListsJson.push_back(output);
          bestBuildListsString.push_back(item);
        }
      }
      delete g;
     }
  }
}

void Genetic::mutation(){

}

void Genetic::reproduction(){

}