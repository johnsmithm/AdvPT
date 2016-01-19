#include "Action.h"
#include "util.h"
#include <string> 
#include <cmath>

using namespace std;


// Factor for build time. This is because of chronoboost
const int FP_BUILDTIME_FACTOR = 10;
const int FP_BUILDTIME_BOOST_FACTOR = 15;


/** @brief checks if an action can be executed
 *
 *  @return true if all dependencies for this action
 *           are met (resources, buildings and units), else false
 */
bool BuildAction::canExecute() {
    return game.getGasAmount() >= objectToBuild.getGasCost() &&
            game.getMineralAmount() >= objectToBuild.getMineralCost() &&
            game.getAvailableSupplyAmount() >= objectToBuild.getSupplyCost() &&
            objectToBuild.areDependenciesMet() &&
            objectToBuild.getPossibleProducer() != nullptr;
}


/** @brief starts the action
 *  Increases busyness of dependencies, subtracts resources and removes
 *  morphed units. Does not recheck canExecute! Do this before calling start.
 */
void BuildAction::start() {
    producingInstance = objectToBuild.getPossibleProducer();

    assert(producingInstance != nullptr);

    if(objectToBuild.getBuildType() == BuildType::MORPH) {
        producingInstance->setDead(true);
    } else if(objectToBuild.getBuildType() == BuildType::ACTIVE_BUILD) {
        producingInstance->increaseBusiness();
    }

    // timeLeft is thenths of a second, buildTime is seconds
    timeLeft = objectToBuild.getBuildTime() * FP_BUILDTIME_FACTOR;
    game.setUsedSupplyAmount(game.getUsedSupplyAmount() + objectToBuild.getSupplyCost());
    game.setGasAmount(game.getGasAmount() - objectToBuild.getGasCost());
    game.setMineralAmount(game.getMineralAmount() - objectToBuild.getMineralCost());

    game.getOutput().event(*this);
}


/** @brief informs the action of a timestep and checks if it is finished
 *
 *  @return true if the action is finished, false if it still needs time
 */
bool BuildAction::timeStep() {
    timeLeft -= producingInstance->isBoostTarget() ? FP_BUILDTIME_BOOST_FACTOR : FP_BUILDTIME_FACTOR;

    return timeLeft <= 0;
}


/** @brief finishs the action
 *  Saves the created instance, decreases the producing instance's busyness
 */
void BuildAction::finish() {
    GameObjectInstance* instance;
    if (objectToBuild.getBuildType() == BuildType::MORPH) {
        objectToBuild.morphInstance(game, *producingInstance);
        instance = producingInstance;
    } else {
        instance = &objectToBuild.addNewInstance(game);
        if (objectToBuild.getBuildType() == BuildType::ACTIVE_BUILD)
            producingInstance->decreaseBusiness();
    }

    game.getOutput().event(*this, instance->getID());
}


int BuildAction::getProducerID() const {
    return (producingInstance == nullptr) ? -1 : producingInstance->getID();
}


void BoostAction::start() {
    target.setBoostTarget(true);

    game.getOutput().event(*this);
}


void MuleAction::start() {
    game.updateMuleAction(1);

    game.getOutput().event(*this);
}


void MuleAction::finish() {
    game.updateMuleAction(-1);
}


void QueenAction::start() {
    game.getOutput().event(*this);
}


void QueenAction::finish() {
    target.setInjectedLarvaeEggs(target.getInjectedLarvaeEggs() - QUEEN_EGGS_AMOUNT);
    target.setOccupiedLarvaSlots(target.getOccupiedLarvaSlots() + QUEEN_EGGS_AMOUNT);
    for (int i = 0; i < QUEEN_EGGS_AMOUNT; ++i)
        game.larva.addNewInstance(game);
}
