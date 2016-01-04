#include "Action.h"

/** @brief checks if an action can be executed
 *
 *  @return true if all dependencies for this action
 *           are met (resources, buildings and units), else false
 */
bool BuildAction::canExecute() {
    return game.getGasAmount() >= objectToBuild->getGasCost() &&
            game.getMineralAmount() >= objectToBuild->getMineralCost() &&
            game.getSupplyAmount() >= objectToBuild->getSupplyCost() &&
            objectToBuild->areDependenciesMet() &&
            objectToBuild->getPossibleProducer() != nullptr;
}


/** @brief starts the action
 *  Increases business of dependencies, subtracts resources and removes
 *  morphed units. Does not recheck canExecute! Do this before calling start.
 */
void BuildAction::start() {;
    producingInstance = objectToBuild->getPossibleProducer();

    if(objectToBuild->getBuildType() == BuildType::MORPH){
        objectToBuild->removeInstance(*producingInstance, game);
    }else if(objectToBuild->getBuildType() == BuildType::ACTIVE_BUILD){
        producingInstance->increaseBusiness();
    }

    timeLeft = objectToBuild->getBuildTime()*10;//timeLeft is thenths of a second, buildTime is seconds
    game.setSupplyAmount(game.getSupplyAmount() - objectToBuild->getSupplyCost());
    game.setGasAmount(game.getGasAmount() - objectToBuild->getGasCost());
    game.setMineralAmount(game.getMineralAmount() - objectToBuild->getMineralCost());
}


/** @brief informs the action of a timestep and checks if it is finished
 *
 *  @return true if the action is finished, false if it still needs time
 */
bool BuildAction::timeStep(){
    timeLeft -= producingInstance->isBoostTarget() ? 15 : 10;

    return timeLeft >= 0;
}

/** @brief finishs the action
 *  Saves the created instance, decreases the producing instance's busyness
 */
void BuildAction::finish(){
    objectToBuild->addNewInstance(game);
    if(objectToBuild->getBuildType() == BuildType::ACTIVE_BUILD){
        producingInstance->decreaseBusiness();
    }
}
