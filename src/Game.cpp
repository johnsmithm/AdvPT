#include <cmath>
#include <iostream>
#include <fstream>
#include <memory>
#include <unordered_set>
#include <cassert>
#include <string>
#include <stdlib.h>
#include <algorithm>
#include "Action.h"
#include "Game.h"
#include "util.h"

using namespace std;


Game::Game(GameObject& mainBuilding, GameObject& worker, GameObject& geyserExploiter)
    : mainBuilding(mainBuilding), worker(worker), geyserExploiter(geyserExploiter) {

    setMineralAmount(INITIAL_MINERAL_AMOUNT);

    // create initial workers and main building
    for (int i = 0; i < INITIAL_WORKER_COUNT; ++i) {
        worker.addNewInstance(getInstances(), *this);
        usedSupply += FP_FACTOR;
    }
    mainBuilding.addNewInstance(getInstances(), *this);
    totalSupply += mainBuilding.getSupplyProvided();
}


bool Game::timeStep() {
    if (currBuildListItem == buildList.end() && (runningBuildActions.size() == 0)) {
        return true;
    }

    minerals += mineralMiningWorkers * mineralsRate;
    minerals += muleActions * MULE_MINING_POWER * mineralsRate;
    gas += gasMiningWorkers * gasRate;

    gasTotalNeded -= mineralMiningWorkers * mineralsRate + muleActions * MULE_MINING_POWER * mineralsRate;
    mineralTotalNeeded -= gasMiningWorkers * gasRate;

    //increase energy on all buildings
    GameObject::increaseInstancesEnergy(getInstances(), energyRate);

    // Iterate over all running build actions and finish them if possible.
    // std::remove_if moves all finished actions to the end of the list and returns an iterator to the first finished action.
    // It does not really delete items from the list, so we have to erase them afterwards.
    auto buildFinishedIt = std::remove_if(runningBuildActions.begin(), runningBuildActions.end(), [&](shared_ptr<BuildAction>& item) {
        if (item->timeStep()) {
            item->finish();
            debugOutput(*item, false);
            return true;
        }
        return false;
    });
    runningBuildActions.erase(buildFinishedIt, runningBuildActions.end());

    // Iterate over all running special actions and finish them if possible.
    // std::remove_if moves all finished actions to the end of the list and returns an iterator to the first finished action.
    // It does not really delete items from the list, so we have to erase them afterwards.
    auto specialFinishedIt = std::remove_if(runningSpecialActions.begin(), runningSpecialActions.end(), [&](shared_ptr<SpecialAction>& item) {
        if (item->timeStep()) {
            item->finish();
            debugOutput(*item, false);
            return true;
        }
        return false;
    });
    runningSpecialActions.erase(specialFinishedIt, runningSpecialActions.end());

    //check buildList
    bool triggeredBuild = false;
    if (currBuildListItem != buildList.end()) {
        if ((**currBuildListItem).tryToStart()) {
            triggeredBuild = true;
            debugOutput(**currBuildListItem, true);
            runningBuildActions.push_back(*currBuildListItem);
            currBuildListItem++;
        }
    }

    // invoke race specific special actions, if no build was triggered
    invokeRaceActions(triggeredBuild);

    // Reassign workers when we finish mining resources for a build item or
    // We have a change in the workers business
    if(finishTimeCurrentBuildItem == curTime || freeWorkers != worker.getFreeInstancesCount(getInstances())
       || previousGeyserExploiterCount != geyserExploiter.getInstancesCount(getInstances())) {
        generateResources();
        previousGeyserExploiterCount = geyserExploiter.getInstancesCount(getInstances());
    }

    // If a new message was created, populate its global entries.
    output.addGameGlobals(*this);

    if (++curTime > 1000)
        throw SimulationException("Maximum timesteps exceeded");

    return false;
}


//For testing
void Game::debugOutput(Action& action, bool start) {
    return;
    cerr << "Time:" << curTime << "\n"
         << "resources{minerals :" << minerals / FP_FACTOR
         << ", vespene :" << gas / FP_FACTOR
         << ", supply-used :" << usedSupply / FP_FACTOR
         << ", supply :" << totalSupply / FP_FACTOR << "}\n"
         << "workers{minerals :" << mineralMiningWorkers
         << ", vespene :" << gasMiningWorkers << "}\n";

    if (start) {
        cerr << "start " << action.getName() << "\n";
    } else {
        cerr << "finish " << action.getName() << "\n";
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
bool Game::precheckBuildList()  {
    unordered_set<string> existing;
    existing.insert(mainBuilding.getName());
    existing.insert(worker.getName());
    existing.insert(GameObject::get("larva").getName());

    int geyserExploiterCount = 0;
    for (auto& item : buildList) {
        auto& obj = item->getObjectToBuild();



        gasTotalNeded += obj.getGasCost();
        mineralTotalNeeded += obj.getMineralCost();

        // check if producer and dependencies are existant
        bool depErr = (obj.getDependencyNames().size() > 0);
        bool prodErr = (obj.getProducerNames().size() > 0);
        for (auto& dep : obj.getDependencyNames()) {
            if (existing.count(dep)) {
                depErr = false;
                break;
            }
        }
        for (auto& prod : obj.getProducerNames()) {
            if (existing.count(prod)) {
                prodErr = false;
                break;
            }
        }
        if (depErr || prodErr)
            return false;

        // if object needs gas ensure that a geyserExploiter is present
        if (obj.getGasCost() > 0 && (geyserExploiterCount == 0))
            return false;

        if (obj.getName() == geyserExploiter.getName()) {
            if (geyserExploiterCount == 2) return false;
            ++geyserExploiterCount;
        }
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

    output.initSimulation(getInstances());

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
                     getMiningTime(right - 1, freeWorkers - (right - 1), neededGas, neededMineral) ? right - 1 : left);
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
    freeWorkers = worker.getFreeInstancesCount(getInstances());

    int neededGas = gasDifference < 0 ? 0 : gasDifference;
    int neededMineral = mineralDifference < 0 ? 0 : mineralDifference;

    bool goTogas = gasTotalNeded > 0;

    if(geyserExploiter.getInstancesCount(getInstances()) > 0){
        /*int timeTotalMiningGas = getMiningTime(min(geyserExploiter.getInstancesCount(getInstances()) * 3, freeWorkers), 0, gasTotalNeded, 0);

        int MaxGasMiningWorkers = min(geyserExploiter.getInstancesCount(getInstances()) * 3, freeWorkers);
        int gasMiningWorkers1 = ternarySearch(0, MaxGasMiningWorkers, neededGas, neededMineral, freeWorkers);
        int mineralMiningWorkers1 = freeWorkers - gasMiningWorkers1;

        int nextAssignment = getMiningTime(gasMiningWorkers1, mineralMiningWorkers1, neededGas, neededMineral);

        goTogas = - curTime -  nextAssignment - timeTotalMiningGas > 0;*/ //need average finishing time
        goTogas = gasTotalNeded > 0;
    }

    if(neededGas == 0 && neededMineral == 0){
        mineralMiningWorkers = freeWorkers;
    } else
        if (neededMineral == 0 || goTogas) {
            gasMiningWorkers = min(geyserExploiter.getInstancesCount(getInstances()) * 3, freeWorkers);
            mineralMiningWorkers = freeWorkers - gasMiningWorkers;

        } else if (neededGas == 0) {
            mineralMiningWorkers = freeWorkers;
            gasMiningWorkers = 0;
        } else {
            int MaxGasMiningWorkers = min(geyserExploiter.getInstancesCount(getInstances()) * 3, freeWorkers);
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
    if (curTime == 1 || mineralMiningWorkers != oldMineralMiningWorkers || gasMiningWorkers != oldGasMiningWorkers)
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
           GameObject::get("extractor")),
      larva(GameObject::get("larva")), queen(GameObject::get("queen")),
      larvaProducerTypes{&GameObject::get("hatchery"), &GameObject::get("lair"), &GameObject::get("hive")} {

    // Add overlord
    auto& overlord = GameObject::get("overlord");
    overlord.addNewInstance(getInstances(), *this);
    setTotalSupplyAmount(getTotalSupplyAmount() + overlord.getSupplyProvided());
    setUsedSupplyAmount(getUsedSupplyAmount() + overlord.getSupplyCost());

    // Add initial larvae
    for (int i = 0; i < 3; ++i)
        larva.addNewInstance(getInstances(), *this);
    assert(mainBuilding.begin(getInstances()) != mainBuilding.end(getInstances()));
    mainBuilding.begin(getInstances())->setOccupiedLarvaSlots(3);
    previousLarvaCount = 3;
}


void ProtossGame::invokeRaceActions(bool buildTriggered) {
    if (!buildTriggered) {
        auto isPotentialBoostTarget = [] (GameObjectInstance &goi) {
            // Note: Would not work properly with buildings that have 2 production lines
            //       but this is not the case in protoss race (where boost action is relevant)
            return (!goi.isBoostTarget()) && (goi.getFreeProductionLines() == 0)
                   && goi.getType().isBuilding();
        };

        for (GameObjectInstance& instance : getInstances()[&mainBuilding]) {
            while (instance.hasEnergy(BOOST_REQUIRED_ENERGY)) {
                vector<GameObjectInstance*> targets = GameObject::getAllInstances(getInstances(), isPotentialBoostTarget);

                if (targets.size() == 0)
                    break;

                auto action = make_shared<BoostAction>(BoostAction(*this, *targets[0], instance));
                Game::debugOutput(*action, true);

                action->start();
                instance.updateEnergy(-BOOST_REQUIRED_ENERGY);

                runningSpecialActions.push_back(action);
            }
        }
    }
}


void TerranGame::invokeRaceActions(bool buildTriggered) {
    if (!buildTriggered) {
        for (GameObjectInstance& instance : getInstances()[&orbitalCommand]) {
            if (instance.hasEnergy(MULE_REQUIRED_ENERGY)) {

                auto action = make_shared<MuleAction>(MuleAction(*this, instance));
                Game::debugOutput(*action, true);

                action->start();
                instance.updateEnergy(-MULE_REQUIRED_ENERGY);

                runningSpecialActions.push_back(action);

            }
        }
    }
}


void ZergGame::invokeRaceActions(bool buildTriggered) {
    // Invoke QueenAction
    if (!buildTriggered) {
        for (GameObjectInstance& instance : getInstances()[&queen]) {
            if (instance.hasEnergy(QUEEN_EGGS_REQUIRED_ENERGY)) {

                auto findTarget = [&]() {
                    GameObjectInstance* possibleTarget = nullptr;
                    unsigned int lowest = MAX_LARVA_SLOTS;
                    for (GameObject* go : larvaProducerTypes) {
                        for (GameObjectInstance& goi : getInstances()[go]) {
                            unsigned int occupied = goi.getOccupiedLarvaSlots() + goi.getInjectedLarvaeEggs();
                            if (occupied < 3) {
                                return &goi;
                            } else if (occupied < lowest) {
                                occupied = lowest;
                                possibleTarget = &goi;
                            }
                        }
                    }
                    return possibleTarget;
                };

                GameObjectInstance* target = findTarget();

                if (target != nullptr) {
                    auto action = make_shared<QueenAction>(QueenAction(*this, instance, *target));
                    Game::debugOutput(*action, true);

                    action->start();
                    instance.updateEnergy(-QUEEN_EGGS_REQUIRED_ENERGY);
                    target->setInjectedLarvaeEggs(target->getInjectedLarvaeEggs() + QUEEN_EGGS_AMOUNT);

                    runningSpecialActions.push_back(action);
                }
            }
        }
    }

    // Get the larvae decrease between this and the previous execution
    unsigned int larvaDecrease = previousLarvaCount - larva.getFreeInstancesCount(getInstances());

    for (GameObject* go : larvaProducerTypes) {
        for (GameObjectInstance& instance : getInstances()[go]) {
            if (larvaDecrease >= instance.getOccupiedLarvaSlots()) {
                larvaDecrease -= instance.getOccupiedLarvaSlots();
                instance.setOccupiedLarvaSlots(0);
            } else {
                instance.setOccupiedLarvaSlots(instance.getOccupiedLarvaSlots() - larvaDecrease);
                larvaDecrease = 0;
            }

            if (instance.getOccupiedLarvaSlots() < LARVA_SLOTS) {
                if (instance.decreaseTimeTillLarvaSpawn() == 0) {
                    instance.setOccupiedLarvaSlots(instance.getOccupiedLarvaSlots() + 1);
                    larva.addNewInstance(getInstances(), *this);
                    instance.resetTimeTillLarvaSpawn();
                }
            }
        }
    }

    // Update the previous larvaCount
    previousLarvaCount = larva.getFreeInstancesCount(getInstances());

}
