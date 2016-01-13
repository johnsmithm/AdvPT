#include "Action.h"
#include "util.h"
#include <string> 
#include <cmath>
using namespace std;
Json::Value& Action::updateMessage(){
    Json::Value& output = game.modifyOutput();
    unsigned int curTime = game.getCurrentTime();

    if(output["messages"] == Json::Value())
        output["messages"] = Json::Value(Json::arrayValue);


    if(output["messages"].size() == 0 || last(output["messages"])["time"] != curTime){
        Json::Value newMessage(Json::objectValue);

        newMessage["time"] = curTime;
        newMessage["events"] = Json::Value(Json::arrayValue);

        output["messages"].append(newMessage);
    }

    Json::Value& message = last(output["messages"]);
    message["events"].append(Json::Value(Json::objectValue));
    return last(message["events"]);
}

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

    if(objectToBuild.getBuildType() == BuildType::MORPH){
        producingInstance->setBusy();
    }else if(objectToBuild.getBuildType() == BuildType::ACTIVE_BUILD){
        producingInstance->increaseBusyness();
    }

    timeLeft = objectToBuild.getBuildTime()*10;//timeLeft is thenths of a second, buildTime is seconds
    game.setUsedSupplyAmount(game.getUsedSupplyAmount() + objectToBuild.getSupplyCost());
    game.setGasAmount(game.getGasAmount() - objectToBuild.getGasCost());
    game.setMineralAmount(game.getMineralAmount() - objectToBuild.getMineralCost());

    Json::Value& curEvent = updateMessage();
    curEvent["type"] = "build-start";
    curEvent["name"] = objectToBuild.getName();
    curEvent["producerID"] = std::to_string(producingInstance->getID());
}


/** @brief informs the action of a timestep and checks if it is finished
 *
 *  @return true if the action is finished, false if it still needs time
 */
bool BuildAction::timeStep(){
    timeLeft -= producingInstance->isBoostTarget() ? 15 : 10;

    return timeLeft <= 0;
}

/** @brief finishs the action
 *  Saves the created instance, decreases the producing instance's busyness
 */
void BuildAction::finish(){
    objectToBuild.addNewInstance(game);
    int producerID = producingInstance->getID();    
    if(objectToBuild.getBuildType() == BuildType::MORPH){
        producingInstance->getType().removeInstance(*producingInstance, game);
    }else if(objectToBuild.getBuildType() == BuildType::ACTIVE_BUILD){
        producingInstance->decreaseBusyness();
    }

    Json::Value& curEvent = updateMessage();
    curEvent["type"] = "build-end";
    curEvent["name"] = objectToBuild.getName();
    curEvent["producerID"] = std::to_string(producerID);
    curEvent["producedIDs"].append(std::to_string(GameObjectInstance::getLastId()));
}


void BoostAction::start(){
    target.setBoostTarget(true);

    Json::Value& curEvent = updateMessage();
    curEvent["type"] = "special";
    curEvent["name"] = "chronoboost";
    curEvent["triggeredBy"] = to_string(source.getID());
    curEvent["targetBuilding"] = to_string(target.getID());
}

void MuleAction::start(){
    game.setMuleAction(1);

    Json::Value& curEvent = updateMessage();
    curEvent["type"] = "special";
    curEvent["name"] = "mule";
    curEvent["triggeredBy"] = to_string(source.getID());
}

void MuleAction::finish(){
    game.setMuleAction(-1);
}
