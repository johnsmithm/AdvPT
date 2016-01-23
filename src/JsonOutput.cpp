#include <string>
#include <iostream>
#include "JsonOutput.h"
#include "util.h"
#include "Game.h"
#include "Action.h"
#include "Const.h"


using namespace std;


/** @brief Outputs general game information. Should be called once at the start.
 *
 *  @param race the race which is currently playing
 */
void JsonOutput::gameInformation(Race race) {
    string raceStr;
    switch(race) {
    case Race::PROTOSS:
        raceStr = "protoss";
        break;
    case Race::ZERG:
        raceStr = "zerg";
        break;
    case Race::TERRAN:
        raceStr = "terran";
        break;
    default:
        raceStr = "unknown";
        break;
    }
    json["game"] = "sc2-hots-" + raceStr;
}


/** @brief Outputs the result of the buildlist precheck.
 *
 *  @param isValid determines if the buildlist is valid or not
 */
void JsonOutput::buildListPrecheck(bool isValid) {
    if (isValid)
        json["buildlistValid"] = 1;
    else
        json["buildlistValid"] = 0;
}


/** @brief Outputs initial units and creates messages array.
 *  Should be only called once per game
 */
void JsonOutput::initSimulation() {
    for (auto goi : GameObject::getAll()) {
        if(goi->getType().getName() != "larva")
            json["initialUnits"][goi->getType().getName()].append(to_string(goi->getID()));
    }

    json["messages"] = Json::Value(Json::arrayValue);
}


/** @brief Creates an empty message. This causes a messge to be created when addGameGlobals is called.
 *  This can be used to create a message when the worker assignment changes and no events are present
 *  in this timestep.
 *
 *  @param game the current game
 */
void JsonOutput::emptyMessage(Game& game) {
    unsigned int curTime = game.getCurrentTime();

    if (json["messages"].size() == 0 || last(json["messages"])["time"] != curTime) {
        Json::Value newMessage(Json::objectValue);
        newMessage["time"] = curTime;
        newMessage["events"] = Json::Value(Json::arrayValue);
        json["messages"].append(newMessage);
    }
}


Json::Value& JsonOutput::basicEvent(Game& game) {
    emptyMessage(game);

    Json::Value& message = last(json["messages"]);
    message["events"].append(Json::Value(Json::objectValue));
    return last(message["events"]);
}


/** @brief Adds a new build action start event.
 *
 *  @param action the current build action
 */
void JsonOutput::event(BuildAction& action) {
    Json::Value& e = basicEvent(action.getGame());
    e["type"] = "build-start";
    e["name"] = action.getObjectToBuild().getName();
    e["producerID"] = to_string(action.getProducerID());
}


/** @brief Adds a new build action finish event.
 *
 *  @param action the current build action
 *  @param producedID the ID of the produced instance
 */
void JsonOutput::event(BuildAction& action, unsigned int producedID) {
    Json::Value& e = basicEvent(action.getGame());
    e["type"] = "build-end";
    e["name"] = action.getObjectToBuild().getName();
    e["producerID"] = to_string(action.getProducerID());
    e["producedIDs"].append(to_string(producedID));
}


/** @brief Adds a new boost action start event.
 *
 *  @param action the current boost action
 */
void JsonOutput::event(BoostAction& action) {
    Json::Value& e = basicEvent(action.getGame());
    e["type"] = "special";
    e["name"] = "chronoboost";
    e["triggeredBy"] = to_string(action.getSource().getID());
    e["targetBuilding"] = to_string(action.getTarget().getID());
}


/** @brief Adds a new mule action start event.
 *
 *  @param action the current mule action
 */
void JsonOutput::event(MuleAction& action) {
    Json::Value& e = basicEvent(action.getGame());
    e["type"] = "special";
    e["name"] = "mule";
    e["triggeredBy"] = to_string(action.getSource().getID());
}


/** @brief Adds a new queen inject larvae action start event.
 *
 *  @param action the current mule action
 */
void JsonOutput::event(QueenAction& action) {
    Json::Value& e = basicEvent(action.getGame());
    e["type"] = "special";
    e["name"] = "injectlarvae";
    e["targetBuilding"] = to_string(action.getTarget().getID());
    e["triggeredBy"] = to_string(action.getSource().getID());
}


/** @brief Adds game global values to the current message if (and only if) there
 *  exists a message at the current timestep. This should be called after all events
 *  at this timestep are produced.
 *
 *  @param game the current game
 */
void JsonOutput::addGameGlobals(Game& game) {
    Json::Value& message = last(json["messages"]);
    if (message["time"] == game.getCurrentTime() || game.getCurrentTime() == 1) {
        message["status"]["resources"]["minerals"] = game.getMineralAmount() / FP_FACTOR;
        message["status"]["resources"]["vespene"] = game.getGasAmount() / FP_FACTOR;
        message["status"]["resources"]["supply"] = game.getTotalSupplyAmount() / FP_FACTOR;
        message["status"]["resources"]["supply-used"] = game.getUsedSupplyAmount() / FP_FACTOR;

        message["status"]["workers"]["minerals"] = game.getMineralMiningWorkers();
        message["status"]["workers"]["vespene"] = game.getGasMiningWorkers();
    }
}


void JsonOutput::print() {
    cout << json;
}

Json::Value JsonOutput::getJson(){
    return json;
}

void JsonOutput::setTime(int value){
    json["time"] = value;
}
