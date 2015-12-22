#include "Game.h"


Game::Game() {//can be done in constructor
    currBuildListItem = &buildList[0];
    mineralsRate = 10;
    gasRate = 10;
}


void Game::timeStep() {
    //if ((*currBuildListItem) == buildList.back())
    //    stop();

    //Add Resurces from previous second
    minerals += mineralsIncrease;
    gas += gasIncrease;

    //increse energy
    //increaseEnergy();

    //check runningAction
    for (Action& item : runningActions) {
        if (/*item.isFinished*/true){
            //item.finish();
            //item.erase();
        } else {
            //item.updateTime();
        }
    }

    //check buildList
    /*if (currBuildListItem->canExecute()) {
        currBuildListItem->start();
        runningActions.push_back(currBuildListItem);
        currBuildListItem++;
    }*/

    assignWorkers();

    //write messages

    ++curTime;
}


void Game::readConfiguration(){
    //read line
    //create GameObject
}


void Game::readBuildList(){
   //read name of item to build
   //find GameObject from techtree
   //construct Action
}


void Game::printOutput(){
//print String
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

