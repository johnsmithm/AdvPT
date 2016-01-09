#include <cmath>
#include <iostream>
#include <fstream>
#include <memory>
#include <unordered_set>

#include <stdlib.h>
#include "Action.h"
#include "Game.h"

//#include <json.h>

using namespace std;


Game::Game(GameObject& mainBuilding, GameObject& worker, GameObject& geyserExploiter)
    : mainBuilding(mainBuilding), worker(worker), geyserExploiter(geyserExploiter) {
    setMineralAmount(50*10000);
    
    for (int i = 0; i < 6; ++i)
        worker.addNewInstance(*this);

    mainBuilding.addNewInstance(*this);
}


bool Game::timeStep() {
    if (currBuildListItem == buildList.end() && runningActions.size() == 0) {
        return true;
    }
    
    minerals += mineralMiningWorkers * mineralsRate;
    gas += gasMiningWorkers * gasRate;

    //increase energy on all buildings
    GameObject::increaseEnergy(energyRate);

    // check runningAction
    std::list<std::shared_ptr<Action>> toRemove;
    for (shared_ptr<Action> item : runningActions) {
        if (item->timeStep()){
            item->finish();
            writeMessages(item,false);
            //runningActions.remove(item);
            toRemove.push_back(item);
        }
    }
    for (shared_ptr<Action> item : toRemove) {
      runningActions.remove(item);
    }

    //check buildList
    if(currBuildListItem != buildList.end()) {
      
      if ((**currBuildListItem).canExecute()) { 
          
          (**currBuildListItem).start();
          writeMessages(*currBuildListItem,true);
          runningActions.push_back(*currBuildListItem);
          currBuildListItem++;
      }
      
      generateResources();
    }


    if(++curTime > 1000)
        throw SimulationException("Maximum timesteps exceeded");

    return false;
}

//For testing
void Game::writeMessages(shared_ptr<Action> action, bool start){
  cout<<"Time:"<<curTime<<"\n"
      << "resourses{minerals :" << minerals / 10000
      << ", vespene :" << gas / 10000 
      << ", supply-used :" <<  usedSupply
      << ", supply :" << totalSupply << "}\n"
      << "workers{minerals :" << mineralMiningWorkers
      << ", vespene :" << gasMiningWorkers<< "}\n";          
  if(start){
    cout<<"start "<<action->getName()<<"\n";
  }
  else {
    cout<<"finish "<<action->getName()<<"\n";
  }
  cout << endl;
}

void Game::readConfiguration(){
    //read line
    //create GameObject
}


void Game::readBuildList(string filename){
    if(filename == "-"){
        readBuildList(cin);
    }else{
        ifstream input(filename);
        if (input.is_open()) {
            readBuildList(input);
            input.close();
        } else {
            cerr << "Cannot open file" << endl;
            exit(1);
        }
    }

}

void Game::readBuildList(istream &input){
    string line;
    int linecounter = 0;
    while(getline(input, line)){
        linecounter++;
        try{
            buildList.push_back(make_shared<BuildAction>(BuildAction(*this, GameObject::get(line))));
            //TODO: check correct race?
        }catch(const out_of_range& e){
            throw SimulationException("Invalid build list: Unknown unit \"" + line + "\" on line " + to_string(linecounter));
        }
    }
}

/** @brief Do quick checks to determine if the current buildList is valid.
 *  Dependencies are checked.
 *  The producer is also checked but there is additional checking at simulation time required.
 *  For all objects that need vespene gas there is also checked that a geyserExploiter
 *  is present.
 *  
 *  @return true if the buildList is valid
 */
bool Game::precheckBuildList(){
    unordered_set<string> existing;
    existing.insert(mainBuilding.getName());
    existing.insert(worker.getName());
    
    bool hasGeyserExploiter = false;
    for (auto item : buildList){
        auto obj = item->getObjectToBuild();

        // check if producer and dependencies are existant
        bool depErr = (obj.getDependencyNames().size() > 0);
        bool prodErr = (obj.getProducerNames().size() > 0);
        for (auto dep : obj.getDependencyNames())
            if (existing.count(dep)) {
                depErr = false;
                break;
            }
        for (auto prod : obj.getProducerNames())
            if (existing.count(prod)) {
                prodErr = false;
                break;
            }
        if (depErr || prodErr)
            return false;

        // if object needs gas ensure that a geyserExploiter is present
        if (obj.getGasCost() > 0 && (!hasGeyserExploiter))
            return false;

        if (obj.getName() == geyserExploiter.getName())
            hasGeyserExploiter = true;
        existing.insert(obj.getName());
    }
    return true;
}

void Game::printOutput(){
  cout<<output;
}

void Game::simulate(){
    assert(buildList.size()!=0);

    if (!precheckBuildList())
        throw SimulationException("BuildList invalid");

    currBuildListItem = buildList.begin();

    while(!timeStep()){};
}


void Game::generateResources() {
    if(currBuildListItem == buildList.end()) {
      return;
    }
    int gasDifference =  (**currBuildListItem).getGasCost() - getGasAmount();
    int mineralDifference = (**currBuildListItem).getMineralCost() - getMineralAmount();

    gasMiningWorkers = 0;
    mineralMiningWorkers = 0;
    unsigned int freeWorkers = worker.getFreeInstancesCount();

    if (gasDifference > 0 && mineralDifference <= 0) {
       gasMiningWorkers = min(geyserExploiter.getInstancesCount() * 3, freeWorkers);
       mineralMiningWorkers = freeWorkers - gasMiningWorkers;
    } else if (mineralDifference > 0 && gasDifference <= 0) {
       mineralMiningWorkers = freeWorkers;
       gasMiningWorkers = 0;
    } else {
       gasMiningWorkers = min(geyserExploiter.getInstancesCount() * 3, (unsigned int)((freeWorkers+1)/2));
       mineralMiningWorkers = freeWorkers - gasMiningWorkers;
    }   
}


ProtosGame::ProtosGame()
    : Game(GameObject::get("nexus"), GameObject::get("probe"),
           GameObject::get("assimilator")) {}

