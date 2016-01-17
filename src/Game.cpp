#include <cmath>
#include <iostream>
#include <fstream>
#include <memory>
#include <unordered_set>

#include <string>
#include <stdlib.h>
#include "Action.h"
#include "Game.h"
#include "util.h"

using namespace std;

Game::Game(GameObject& mainBuilding, GameObject& worker, GameObject& geyserExploiter)
    : mainBuilding(mainBuilding), worker(worker), geyserExploiter(geyserExploiter) {

    setMineralAmount(INITIAL_MINERAL_AMOUNT);
    
    // create initial workers and main building
    for (int i = 0; i < INITIAL_WORKER_COUNT; ++i) {
        worker.addNewInstance(*this);
        ++usedSupply;
    }
    mainBuilding.addNewInstance(*this);
}


bool Game::allBuildActionsFinished() {
    auto predicate = [](shared_ptr<Action> a) {
        return a->isBuildAction();
    };
    if (findFirstIf(runningActions, predicate) == nullptr)
        return true;
    return false;
}


bool Game::timeStep() {
    if (currBuildListItem == buildList.end() && allBuildActionsFinished()) {
        return true;
    }

    minerals += mineralMiningWorkers * mineralsRate;
    minerals += muleActions * MULE_MINING_POWER * mineralsRate;
    gas += gasMiningWorkers * gasRate;

    //increase energy on all buildings
    GameObject::increaseInstancesEnergy(energyRate);

    // check runningAction
    std::list<std::shared_ptr<Action>> toRemove;
    for (shared_ptr<Action> item : runningActions) {
        if (item->timeStep()) {

            item->finish();
            debugOutput(item, false);
            toRemove.push_back(item);
        }
    }
    for (shared_ptr<Action> item : toRemove) {
        runningActions.remove(item);
    }

    //check buildList
    bool triggeredBuild = false;
    if (currBuildListItem != buildList.end()) {
        if ((**currBuildListItem).canExecute()) {

            (**currBuildListItem).start();
            triggeredBuild = true;
            debugOutput(*currBuildListItem, true);
            runningActions.push_back(*currBuildListItem);
            currBuildListItem++;
        }
    }

    // invoke race specific special actions, if no build was triggered
    if(!triggeredBuild)
        invokeSpecial();

    // Reassign workers when we finish mining resources for a build item or
    // We have a change in the workers business
    if(finishTimeCurrentBuildItem == curTime || freeWorkers != worker.getFreeInstancesCount()){
        generateResources();
    }

    // If a new message was created, populate its global entries.
    output.addGameGlobals(*this);

    if (++curTime > 1000)
        throw SimulationException("Maximum timesteps exceeded");

    return false;
}


//For testing
void Game::debugOutput(shared_ptr<Action> action, bool start) {
    cerr << "Time:" << curTime << "\n"
         << "resources{minerals :" << minerals / FP_FACTOR
         << ", vespene :" << gas / FP_FACTOR
         << ", supply-used :" << usedSupply
         << ", supply :" << totalSupply << "}\n"
         << "workers{minerals :" << mineralMiningWorkers
         << ", vespene :" << gasMiningWorkers << "}\n";

    if (start) {
        cerr << "start " << action->getName() << "\n";
    } else {
        cerr << "finish " << action->getName() << "\n";
    }

    cerr << endl;
}


void Game::readBuildList(string filename) {
    if (filename == "-") {
        readBuildList(cin);
    } else {
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


void Game::readBuildList(istream &input) {
    string line;
    int linecounter = 0;
    while (getline(input, line)) {
        linecounter++;
        try {
            buildList.push_back(make_shared<BuildAction>(BuildAction(*this, GameObject::get(line))));
            //TODO: check correct race?
        } catch (const out_of_range& e) {
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
bool Game::precheckBuildList() const {
    unordered_set<string> existing;
    existing.insert(mainBuilding.getName());
    existing.insert(worker.getName());
    existing.insert(GameObject::get("larva").getName());

    bool hasGeyserExploiter = false;
    for (auto item : buildList) {
        auto obj = item->getObjectToBuild();

        // check if producer and dependencies are existant
        bool depErr = (obj.getDependencyNames().size() > 0);
        bool prodErr = (obj.getProducerNames().size() > 0);
        for (auto dep : obj.getDependencyNames()) {
            if (existing.count(dep)) {
                depErr = false;
                break;
            }
        }
        for (auto prod : obj.getProducerNames()) {
            if (existing.count(prod)) {
                prodErr = false;
                break;
            }
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


void Game::simulate() {
    assert(buildList.size() != 0);

    output.gameInformation(getRace());

    if (!precheckBuildList()) {
        output.buildListPrecheck(false);
        throw SimulationException("BuildList invalid");
    }

    output.buildListPrecheck(true);

    output.initSimulation();

    currBuildListItem = buildList.begin();

    while (!timeStep());
}


/**
 *@brief Calculate maximum time of harvesting.
 *
 * @return time.
 */
int Game::getMiningTime(int gasMiningWorkers, int mineralMiningWorkers, int neededGas, int neededMineral) {
    assert(gasMiningWorkers >= 0 && mineralMiningWorkers >= 0);
    assert(neededGas >= 0 && neededMineral >= 0);

    //Todo add mule thing
    if (neededGas == 0 && neededMineral == 0) {
        return 0;
    }
    if (neededGas > 0 && gasMiningWorkers == 0) {
        return (1 << 25);
    }
    if (neededMineral > 0 && mineralMiningWorkers == 0) {
        return (1 << 25);
    }

    int gasPerSecond = gasRate * gasMiningWorkers;
    int mineralPerSecond = mineralsRate * mineralMiningWorkers;

    int timeMineral = mineralPerSecond != 0 ? neededMineral / mineralPerSecond : 0;
    int timeGas = gasPerSecond != 0 ? neededGas / gasPerSecond : 0;

    return max(((gasPerSecond ? neededGas % gasPerSecond : 1) == 0 ? timeGas : timeGas + 1),
               ((mineralPerSecond ? neededMineral % mineralPerSecond : 1) == 0 ? timeMineral : timeMineral + 1));
}


/**
 * @brief Calculate optimum worker assignment.
 * The function f(gasWokers, mineralWorker, neededGas, neededMineral) =
 * max(neededGas / gasWokers * rateGas, neededMineral / mineralWorker * rateMineral)
 * f is convex => we can find the minimum using ternary search - O(log interval).
 *
 * @param left - interval begin.
 * @param right - interval end.
 * @param neededGas
 * @param neededMineral
 * @param freeWorkers
 *
 * @return gasMiningWorkers.
 */
int Game::ternarySearch(int left, int right, int neededGas, int neededMineral, int freeWorkers) {
    assert(left >= 0 && right >= left);
    assert(neededGas >= 0 && neededMineral >= 0);
    assert(freeWorkers >= 0);
    if (right - left == 0) {
        return right;
    }
    if (right - left == 1) {
        return (getMiningTime(left, freeWorkers - left, neededGas, neededMineral) >
                getMiningTime(right, freeWorkers - right, neededGas, neededMineral) ? right : left);
    }
    if (right - left == 2) {
        int inner = (getMiningTime(left, freeWorkers - left, neededGas, neededMineral) >
                     getMiningTime(right - 1, freeWorkers - right - 1, neededGas, neededMineral) ? right - 1 : left);
        return (getMiningTime(inner, freeWorkers - inner, neededGas, neededMineral) >
                getMiningTime(right, freeWorkers - right, neededGas, neededMineral) ? right : inner);
    }
    int m1 = left + (right - left) / 3;
    int m2 = left + 2 * (right - left) / 3;

    if (getMiningTime(m1, freeWorkers - m1, neededGas, neededMineral) >
        getMiningTime(m2, freeWorkers - m2, neededGas, neededMineral)) {
        return ternarySearch(m1, right, neededGas, neededMineral, freeWorkers);
    } else {
        return ternarySearch(left, m2, neededGas, neededMineral, freeWorkers);
    }
}


void Game::generateResources() {
    std::vector<std::shared_ptr<BuildAction>>::iterator getResourcesBuildListItem = currBuildListItem;
    if(getResourcesBuildListItem == buildList.end()){
        return;
    }
    //ToDo assign-changes just at events or write messages when reassign workers and
    // there are no events
    int gasDifference = (**getResourcesBuildListItem).getGasCost() - getGasAmount();
    int mineralDifference = (**getResourcesBuildListItem).getMineralCost() - getMineralAmount();

    // If we have enough resources, we move to next item
    while ((*getResourcesBuildListItem != buildList.back()) && gasDifference <= 0 && mineralDifference <= 0) {
        getResourcesBuildListItem++; //print event
        gasDifference += (**getResourcesBuildListItem).getGasCost();
        mineralDifference += (**getResourcesBuildListItem).getMineralCost();
    }
    unsigned int oldGasMiningWorkers = gasMiningWorkers;
    unsigned int oldMineralMiningWorkers = mineralMiningWorkers;
    gasMiningWorkers = 0;
    mineralMiningWorkers = 0;
    freeWorkers = worker.getFreeInstancesCount();

    int neededGas = gasDifference < 0 ? 0 : gasDifference;
    int neededMineral = mineralDifference < 0 ? 0 : mineralDifference;

    if(neededGas == 0 && neededMineral == 0){
        mineralMiningWorkers = freeWorkers;
    } else
        if (neededMineral == 0) {
            gasMiningWorkers = min(geyserExploiter.getInstancesCount() * 3, freeWorkers);
            mineralMiningWorkers = freeWorkers - gasMiningWorkers;

        } else if (neededGas == 0) {
            mineralMiningWorkers = freeWorkers;
            gasMiningWorkers = 0;
        } else {
            int MaxGasMiningWorkers = min(geyserExploiter.getInstancesCount() * 3, freeWorkers);
            gasMiningWorkers = ternarySearch(0, MaxGasMiningWorkers, neededGas, neededMineral, freeWorkers);
            mineralMiningWorkers = freeWorkers - gasMiningWorkers;
        }
    if(neededGas == 0 && neededMineral == 0){
        finishTimeCurrentBuildItem = 1001;
    }
    else {
        finishTimeCurrentBuildItem = curTime +
            getMiningTime(gasMiningWorkers, mineralMiningWorkers, neededGas, neededMineral);
    }

    // Write messages when we reassign workers and do not have events
    if (mineralMiningWorkers != oldMineralMiningWorkers || gasMiningWorkers != oldGasMiningWorkers)
        output.emptyMessage(*this);
}


ProtossGame::ProtossGame()
    : Game(GameObject::get("nexus"), GameObject::get("probe"),
           GameObject::get("assimilator")) {}


TerranGame::TerranGame()
    : Game(GameObject::get("command_center"), GameObject::get("scv"),
           GameObject::get("refinery")), orbitalCommand(GameObject::get("orbital_command")) {}


ZergGame::ZergGame()
    : Game(GameObject::get("hatchery"), GameObject::get("drone"),
           GameObject::get("extractor")), larva(GameObject::get("larva")),
      larvaProducerTypes{&GameObject::get("hatchery"), &GameObject::get("lair"), &GameObject::get("hive")} {
    // Add overlord
    GameObject::get("overlord").addNewInstance(*this);
    // Add initial larvae
    for (int i = 0; i < 3; ++i)
        larva.addNewInstance(*this);
    larvaProducerProperties.emplace_back();
    larvaProducerProperties[0].occupiedSlots = 3;
    previousLarvaCount = 3;
}


void ProtossGame::invokeSpecial() {
    auto isPotentialBoostTarget = [] (GameObjectInstance &goi) {
        // Note: Would not work properly with buildings that have 2 production lines
        //       but this is not the case in protoss race (where boost action is relevant)
        return (!goi.isBoostTarget()) && (goi.getFreeProductionLines() == 0) && goi.getType().isBuilding();
    };

    for (GameObjectInstance& instance : mainBuilding) {
        while (instance.hasEnergy(BOOST_REQUIRED_ENERGY)) {
            vector<GameObjectInstance*> targets = GameObject::getAll(isPotentialBoostTarget);

            if (targets.size() == 0)
                break;

            shared_ptr<BoostAction> action = make_shared<BoostAction>(BoostAction(*this, *targets[0], instance));
            Game::debugOutput(action, true);

            action->start();
            instance.updateEnergy(-BOOST_REQUIRED_ENERGY);

            runningActions.push_back(action);
        }
    }

}


void TerranGame::invokeSpecial() {
    for (GameObjectInstance& instance : orbitalCommand) {
        if (instance.hasEnergy(MULE_REQUIRED_ENERGY)) {

            shared_ptr<MuleAction> action = make_shared<MuleAction>(MuleAction(*this, instance));
            Game::debugOutput(action, true);

            action->start();
            instance.updateEnergy(-MULE_REQUIRED_ENERGY);

            runningActions.push_back(action);

        }
    }
}


void ZergGame::invokeSpecial() {
    // Add recently built larva producers
    unsigned int producerCount = 0;
    unsigned int previousProducerCount = larvaProducerProperties.size();
    for (GameObject* go : larvaProducerTypes)
        producerCount += go->getInstancesCount();
    for (unsigned int i = previousProducerCount; i < producerCount; ++i)
        larvaProducerProperties.emplace_back();

    // Get the larvae decrease between this and the previous execution
    // TODO: this will have to be changed when the SpawnLarvaeAction is implemented
    unsigned int larvaDecrease = previousLarvaCount - larva.getFreeInstancesCount();

    // Subtract larvaDecrease and create new larvae
    for (LarvaProducerProperties& properties : larvaProducerProperties) {
        if (larvaDecrease >= properties.occupiedSlots) {
            larvaDecrease -= properties.occupiedSlots;
            properties.occupiedSlots = 0;
        } else {
            properties.occupiedSlots -= larvaDecrease;
            larvaDecrease = 0;
        }

        --properties.timeTillSpawn;
        if ((properties.occupiedSlots < LARVA_SLOTS) && (properties.timeTillSpawn == 0)) {
            ++properties.occupiedSlots;
            larva.addNewInstance(*this);
        }

        if (properties.timeTillSpawn == 0)
            properties.timeTillSpawn = LARVA_SPAWN_TIME;
    }

    // Update the previous larvaCount - this must be set after calls to larva.addNewInstance!
    previousLarvaCount = larva.getFreeInstancesCount();
}
