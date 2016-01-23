#ifndef _GENETIC_H_
#define _GENETIC_H_


#include <memory>
#include "Game.h"
#include "GameObject.h"
#include <string> 

#include<vector>

#include "json/json.h"

using namespace std;

class Genetic{
  public:
    Genetic(std::string setRace, std::string setTypeSearch, std::string setUnit);
    void findBuildList();
    void printBest();
  private:
  	void getNeededUnits();

    void makeBuildList();
    void selection();
    void mutation();
    void reproduction();

    void getDependency(string go);
    bool exist(vector<string> v,string name);
    std::string race;
    std::string typeSearch;

    GameObject& unit;
    std::vector<std::string> involvedUnits;  
    std::vector<std::string> dependencyUnits;  
    std::vector<std::string> resourcesUnits;    
    std::vector<std::vector<std::string>> requiredOrder;

    vector<vector<string>> allBuildLists;

    vector<vector<string>> bestBuildListsString;
    vector<Json::Value> bestBuildListsJson;

};

#endif