#ifndef _GAME_H_
#define _GAME_H_

#include <cassert>
#include <vector>
#include <string>
#include <list>
#include <memory>
#include <istream>
#include "GameObject.h"
#include "JsonOutput.h"
#include "Const.h"


//forward declare classes to fix cyclic dependencies
class Action;
class SpecialAction;
class BuildAction;


class SimulationException : public std::runtime_error {
public:
    SimulationException(const std::string &msg)
        : std::runtime_error(msg) {};
};


class Game {
public:

    virtual ~Game() {};
    unsigned int getMineralAmount() const {return minerals;}
    unsigned int getMineralMiningWorkers() const {return mineralMiningWorkers;}
    unsigned int getGasAmount() const {return gas;}
    unsigned int getGasMiningWorkers() const {return gasMiningWorkers;}
    unsigned int getUsedSupplyAmount() const {return usedSupply;}
    unsigned int getTotalSupplyAmount() const {return totalSupply;}
    unsigned int getAvailableSupplyAmount() const {return totalSupply-usedSupply;}
    unsigned int getCurrentTime() const {return curTime;};

    void setMineralAmount(int amount) {minerals = amount;}
    void setGasAmount(int amount) {gas = amount;}
    void setUsedSupplyAmount(int amount) {usedSupply = amount;}
    void setTotalSupplyAmount(int amount) {totalSupply = amount;}

    unsigned int newInstanceID() {return nextInstanceID++;}

    void readBuildList(std::string filename);

    bool precheckBuildList() const;

    void updateMuleAction(int value) {muleActions += value;}

    void simulate();

    JsonOutput& getOutput() {return output;}

    virtual Race getRace() const = 0;

protected:
    Game(GameObject& mainBuilding, GameObject& worker, GameObject& geyserExploiter);

    GameObject& mainBuilding;
    GameObject& worker;
    GameObject& geyserExploiter;

    std::list<std::shared_ptr<SpecialAction>> runningSpecialActions;

    void debugOutput(Action& action, bool start);

    virtual void invokeRaceActions(bool buildTriggered) = 0;

private:
    unsigned int curTime = 1;

    unsigned int minerals = 0;
    unsigned int gas = 0;
    unsigned int usedSupply = 0;
    unsigned int totalSupply = 0;

    unsigned int gasMiningWorkers = 0;
    unsigned int mineralMiningWorkers = 6;

    unsigned int muleActions = 0;
    unsigned int freeWorkers = 6;

    unsigned int finishTimeCurrentBuildItem = 1;
    unsigned int previousGeyserExploiterCount = 0;

    unsigned int nextInstanceID = 0;

    const int mineralsRate = DEFAULT_WORKER_MINERAL_INCREASE;
    const int gasRate = DEFAULT_WORKER_GAS_INCREASE;
    const int energyRate = DEFAULT_ENERGY_INCREASE;

    std::vector<std::shared_ptr<BuildAction>> buildList;
    std::vector<std::shared_ptr<BuildAction>>::iterator currBuildListItem;

    std::list<std::shared_ptr<BuildAction>> runningBuildActions;

    JsonOutput output;

    void readBuildList(std::istream &input);
    bool timeStep();
    void generateResources();
    bool allBuildActionsFinished();
    void updateMessagesForWorkersReassignment();
    int ternarySearch(int left, int right, int neededGas,int neededMineral, int freeWorkers);
    int getMiningTime(int gasMiningWorkers, int mineralMiningWorkers, int neededGas,int neededMineral);
};


class ProtossGame : public Game {
public:
    ProtossGame();

    virtual Race getRace() const {return Race::PROTOSS;}

protected:
    virtual void invokeRaceActions(bool buildTriggered) override;
};


class TerranGame : public Game {
public:
    TerranGame();

    virtual Race getRace() const {return Race::TERRAN;}

protected:
    GameObject& orbitalCommand;

    virtual void invokeRaceActions(bool buildTriggered) override;
};


class ZergGame : public Game {
public:
    GameObject& larva;
    GameObject& queen;    

    ZergGame();

    virtual Race getRace() const {return Race::ZERG;}
    unsigned int getPreviousLarvaCount() {return previousLarvaCount;}
    void setPreviousLarvaCount(unsigned int value) {previousLarvaCount = value;}

protected:
    virtual void invokeRaceActions(bool buildTriggered) override;

private:
    std::vector<GameObject*> larvaProducerTypes;
    unsigned int previousLarvaCount = 0;
};

#endif
