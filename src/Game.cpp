#include <iostream>
#include <fstream>
#include <memory>

#include "Action.h"
#include "Game.h"

using namespace std;

Game::Game() {//can be done in constructor
    mineralsRate = 10;
    gasRate = 10;
}


bool Game::timeStep() {
    if (currBuildListItem == buildList.end())
        return true;

    //Add Resurces from previous second
    minerals += mineralsIncrease;
    gas += gasIncrease;

    //increse energy on all buildings
    GameObject::increaseEnergy();

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

    // assignWorkers();

    //write messages

    ++curTime;

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

    while(timeStep()){};
}


void Game::assignWorkers() {//naive
    /*if (!needGas(currBuildListItem)) {
        //All workers harv minerals
        mineralMiningWorkers += freeWorkers + gasMiningWorkers;
    } else {
        //Maximum numbers of workers go to gas, others go to minerals
        freeWorkers += mineralMiningWorkers + gasMiningWorkers;
        if (exploitedGeysers * 3 <= freeWorkers) {
            //we have more workers than Geysers need
            gasMiningWorkers = exploitedGeysers * 3;
            freeWorkers -= gasMiningWorkers;
            mineralMiningWorkers = freeWorkers;
        } else {
            gasMiningWorkers = freeWorkers;
            mineralMiningWorkers = 0;
        }
    }
    freeWorkers = 0;
    mineralsIncrise = mineralMiningWorkers * mineralsRate + MuleIncrese;
    gasIncrise = gasMiningWorkers * gasRate;*/
}

