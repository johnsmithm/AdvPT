#include "Action.h"

bool BuildAction::canExecute() {
    for(GameObject objectType : objectToBuild.possibleProducers) {
        for(GameObjectInstance object : objectType.instances) {
            if(!object.isBusy() && ressourcesExist()){
                producingInstance = object;
                producingInstance.setBusy();
                return true;
            }
        }
    }
}


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
}
