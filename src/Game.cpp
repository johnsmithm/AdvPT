#include <cmath>
#include <iostream>
#include <fstream>
#include <memory>
#include <unordered_set>

#include <stdlib.h>
#include "Action.h"
#include "Game.h"

//#include <json.h>

using namespace std;

Game::Game(GameObject& mainBuilding, GameObject& worker, GameObject& geyserExploiter)
: mainBuilding(mainBuilding), worker(worker), geyserExploiter(geyserExploiter) {
  setMineralAmount(50 * 10000);

  for (int i = 0; i < 6; ++i)
    worker.addNewInstance(*this);

  mainBuilding.addNewInstance(*this);
}

bool Game::timeStep() {
  if (currBuildListItem == buildList.end() && runningActions.size() == 0) {
    return true;
  }

  minerals += mineralMiningWorkers * mineralsRate;
  gas += gasMiningWorkers * gasRate;

  //increase energy on all buildings
  GameObject::increaseEnergy(energyRate);

  // check runningAction
  std::list<std::shared_ptr < Action>> toRemove;
  for (shared_ptr<Action> item : runningActions) {
    if (item->timeStep()) {
      item->finish();
      debugOutput(item, false);
      //runningActions.remove(item);
      toRemove.push_back(item);
    }
  }
  for (shared_ptr<Action> item : toRemove) {
    runningActions.remove(item);
  }

  //check buildList
  if (currBuildListItem != buildList.end()) {

    if ((**currBuildListItem).canExecute()) {

      (**currBuildListItem).start();
      debugOutput(*currBuildListItem, true);
      runningActions.push_back(*currBuildListItem);
      currBuildListItem++;
    }

    generateResources();
  }

  // invoke race specific special actions
  invokeSpecial();

  if (++curTime > 1000)
    throw SimulationException("Maximum timesteps exceeded");

  return false;
}

//For testing

void Game::debugOutput(shared_ptr<Action> action, bool start) {
  cerr << "Time:" << curTime << "\n"
          << "resourses{minerals :" << minerals / 10000
          << ", vespene :" << gas / 10000
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

void Game::readConfiguration() {
  //read line
  //create GameObject
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
bool Game::precheckBuildList() {
  unordered_set<string> existing;
  existing.insert(mainBuilding.getName());
  existing.insert(worker.getName());

  bool hasGeyserExploiter = false;
  for (auto item : buildList) {
    auto obj = item->getObjectToBuild();

    // check if producer and dependencies are existant
    bool depErr = (obj.getDependencyNames().size() > 0);
    bool prodErr = (obj.getProducerNames().size() > 0);
    for (auto dep : obj.getDependencyNames())
      if (existing.count(dep)) {
        depErr = false;
        break;
      }
    for (auto prod : obj.getProducerNames())
      if (existing.count(prod)) {
        prodErr = false;
        break;
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

void Game::printOutput() {
  cout << output;
}

void Game::simulate() {
  assert(buildList.size() != 0);

  if (!precheckBuildList())
    throw SimulationException("BuildList invalid");

  getResoursesBuildListItem = currBuildListItem = buildList.begin();

  while (!timeStep()) {
  };
}

/**
 *@brief Calculate maximum time of harvesting.
 * 
 * @return time.
 */
int Game::getMiningTime(int gasMiningWorkers, int mineralMiningWorkers, int neededGas, int neededMineral) {
  assert(gasMiningWorkers >= 0 && mineralMiningWorkers >= 0);
  assert(neededGas >= 0 && neededMineral >= 0);

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
  if (getResoursesBuildListItem == buildList.end()) {
    return;
  }

  int gasDifference = (**getResoursesBuildListItem).getGasCost() - getGasAmount();
  int mineralDifference = (**getResoursesBuildListItem).getMineralCost() - getMineralAmount();

  // If we have enough resources, we move to next item
  while (gasDifference <= 0 && mineralDifference <= 0) {
    getResoursesBuildListItem++;
    if (getResoursesBuildListItem == buildList.end()) {
      return;
    }
    gasDifference += (**getResoursesBuildListItem).getGasCost();
    mineralDifference += (**getResoursesBuildListItem).getMineralCost();
  }

  gasMiningWorkers = 0;
  mineralMiningWorkers = 0;
  unsigned int freeWorkers = worker.getFreeInstancesCount();

  if (gasDifference > 0 && mineralDifference <= 0) {
    gasMiningWorkers = min(geyserExploiter.getInstancesCount() * 3, freeWorkers);
    mineralMiningWorkers = freeWorkers - gasMiningWorkers;

  } else if (mineralDifference > 0 && gasDifference <= 0) {
    mineralMiningWorkers = freeWorkers;
    gasMiningWorkers = 0;
  } else {
    int MaxGasMiningWorkers = min(geyserExploiter.getInstancesCount() * 3, freeWorkers);
    int neededGas = gasDifference < 0 ? 0 : gasDifference;
    int neededMineral = mineralDifference < 0 ? 0 : mineralDifference;
    gasMiningWorkers = ternarySearch(0, MaxGasMiningWorkers, neededGas, neededMineral, freeWorkers);
    mineralMiningWorkers = freeWorkers - gasMiningWorkers;
  }
}

ProtosGame::ProtosGame()
: Game(GameObject::get("nexus"), GameObject::get("probe"),
GameObject::get("assimilator")) {
}

bool getNonBoostedBuildings(GameObjectInstance &goi) {
  return !goi.isBoostTarget() && goi.getBusyness() >= 0 && goi.getType().isBuilding();
}

void ProtosGame::invokeSpecial() {
  for (GameObjectInstance& instance : GameObject::get("nexus").getAllInstances()) {
    while (instance.hasEnergy(25 * 10000)) {
      vector<GameObjectInstance*> targets = GameObject::getAll(getNonBoostedBuildings);

      if (targets.size() > 0) {
        shared_ptr<BoostAction> action = make_shared<BoostAction>(BoostAction(*this, *targets[0]));
        Game::debugOutput(action, true);

        action->start();
        instance.updateEnergy(-25 * 10000);

        runningActions.push_back(action);
      }
    }
  }
}

