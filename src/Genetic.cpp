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
{}

/**
 * 
 */
void Genetic::findBuildList(){
  getNeededUnits();
  makeBuildList();
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

    cout<<"name|beginTime|mine|gas\n";
    for(size_t i=0; i<bestBuildListsString[0].size(); ++i)
    	if(bTime.size() > i)
    	cout<<bestBuildListsString[0][i]<<" "<<bTime[i]<<" "<<minerals[i]<<" "<<gas[i]<<"\n";
    cout<<"Time:"<<bestBuildListsJson[0]["time"]<<"\n";
  }
}

bool Genetic::exist(vector<string> v,string name){
	if(v.end() != find(v.begin(),v.end(), name)){
     	return true;
     }
    return false; 
}

void Genetic::getDependency(string name){
     if(exist(involvedUnits,name) || name == "probe" || name == "nexus")
       return;     

     involvedUnits.push_back(name);
     GameObject& go = GameObject::get(name);
     
     for(auto item : go.getProducerNames()){
       	getDependency(item);
     }

     for(auto item : go.getDependencyNames()){
       	getDependency(item);
     }
    requiredOrder.push_back(requiredOrder[requiredOrder.size()-1]);
    requiredOrder[requiredOrder.size()-1].push_back("assimilator");
    for(size_t i=0; i<requiredOrder.size(); ++i)
       requiredOrder[i].push_back(name);
}

void Genetic::getNeededUnits(){
  std::vector<string> v;
  requiredOrder.push_back(v);
  getDependency(unit.getName());
  requiredOrder.erase(requiredOrder.begin());

  resourcesUnits.push_back("probe");
  resourcesUnits.push_back("nexus");
  resourcesUnits.push_back("pylon");
  resourcesUnits.push_back("assimilator");
  /*for(auto item1 : requiredOrder){
  	for(auto item : item1)
      cout<<item<<"\n";
  	cout<<"\n";
  }*/
}


void Genetic::makeBuildList(){
  //int supply = 4;
  srand (time(NULL));

  allBuildLists.clear();
  for(auto item : requiredOrder)
	  for (int i = 0; i < 10 ; ++ i){
	  	std::vector<std::string> mylist = item;
	  	for (int j=0 ; j <= i; ++j){
	  		int pos = rand() % (mylist.size()-1) ;
	  		mylist.insert(mylist.begin()+pos, "probe");
	  	} 
	    allBuildLists.push_back(mylist);
	    mylist.clear();
	  } 
	//  allBuildLists.clear();
//allBuildLists.push_back({"probe","probe","probe","assimilator","probe","pylon","probe","assimilator","gateway","assimilator","probe","cybernetics_core","robotics_facility","robotics_bay","colossus"});	
	}

/*
void Genetic::makeBuildList(){
  //int supply = 4;
  srand (time(NULL));
  allBuildLists.clear();  
  std::vector<std::string> mylist;
  for(auto item : requiredOrder) {
	  for (int i = 0; i < item.size() ; ++ i){
	  	int coin = rand() % 2 ;
	  	mylist.push_back( "probe");
	  } 
	  mylist.push_back(item[i]);
	  allBuildLists.push_back(mylist);
	  mylist.clear();
   }
}
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
  	 	if(bestBuildListsJson.size()>0){
         if(bestBuildListsJson[0]["time"] > output["time"]){     
  	 		bestBuildListsJson.clear();
  	 		bestBuildListsString.clear();    	
  	 		bestBuildListsJson.push_back(output);
  	 		bestBuildListsString.push_back(item);
         } 
  	 	}else{
  	 		bestBuildListsJson.clear();
  	 		bestBuildListsString.clear();
  	 		bestBuildListsJson.push_back(output);
  	 		bestBuildListsString.push_back(item);
  	 	}
  	 	delete g;
  	 }
  }
}

void Genetic::mutation(){

}

void Genetic::reproduction(){

}