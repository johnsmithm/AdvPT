#include <cmath>
#include <iostream>
#include <fstream>
#include <memory>

#include "Action.h"
#include "Game.h"

using namespace std;


bool Game::timeStep() {
    if (currBuildListItem == buildList.end())
        return true;

    //increase energy on all buildings
    GameObject::increaseEnergy(energyRate);

    // check runningAction
    for (shared_ptr<Action> item : runningActions) {
        if (item->timeStep()){
            item->finish();
            runningActions.remove(item);
        }
    }

    //check buildList
    if ((**currBuildListItem).canExecute()) {
        (**currBuildListItem).start();
        runningActions.push_back(*currBuildListItem);
        currBuildListItem++;
    }

    generateResources();

    if(++curTime > 1000)
        throw SimulationException("Maximum timesteps exceeded");

    return false;
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
            buildList.push_back(make_shared<BuildAction>(BuildAction(*this, GameObject::getGameObject(line))));
            //TODO: check correct race?
        }catch(const out_of_range& e){
            throw SimulationException("Invalid build list: Unknown unit \"" + line + "\" on line " + to_string(linecounter));
        }
    }
}


void Game::printOutput(){
//print String
}

void Game::simulate(){
    assert(buildList.size()!=0);

    currBuildListItem = buildList.begin();

    while(!timeStep()){};
}


void Game::generateResources() {
    int gasDifference =  (**currBuildListItem).getGasCost() - getGasAmount();
    int mineralDifference = (**currBuildListItem).getMineralCost() - getMineralAmount();

    unsigned int gasMiningWorkers = 0;
    unsigned int  mineralMiningWorkers = 0;
    unsigned int freeWorkers = GameObject::getFreeWorkerCount();

    if (gasDifference > 0 && mineralDifference <= 0) {
       gasMiningWorkers = min(exploitedGeysers * 3, freeWorkers);
       mineralMiningWorkers = freeWorkers - gasMiningWorkers;
    } else if (mineralDifference > 0 && gasDifference <= 0) {
       mineralMiningWorkers = freeWorkers;
       gasMiningWorkers = 0;
    } else {
       gasMiningWorkers = min(exploitedGeysers * 3, (unsigned int)((freeWorkers+1)/2));
       mineralMiningWorkers = freeWorkers - gasMiningWorkers;
    }

    minerals += mineralMiningWorkers * mineralsRate;
    gas += gasMiningWorkers * gasRate;
}

