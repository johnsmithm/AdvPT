#include "Action.h"

bool BuildAction::canExecute() {
    GameObjectInstance *producer;
    return game.getGasAmount() >= objectToBuild->getGasCost() &&
            game.getMineralAmount() >= objectToBuild->getMineralCost() &&
            game.getSupplyAmount() >= objectToBuild->getSupplyCost() &&
            objectToBuild->areDependenciesMet() &&
            objectToBuild->getPossibleProducer(producer);
}

/*
void BuildAction::start() {
    if(objectToBuild.buildType==INSTANTIATE){
        producingInstance.setFree();
        //Eliminate that worker for that second from harving minerals or gas
    }
    if(race==Zerg && objectToBuild.morphFromLarvae()){
        putNewLarva();
    }
    CalculateFinishingTime();//checkMule
}


void BuildAction::finish() {
    switch(objectToBuild.buildType) {
    case BuildType::MORPH :
        producingInstance.removeInstance();
    case BuildType::ACTIVE_BUILD :
        producingInstance.setFree();
    }
}*/
