#ifndef _GAME_H_
#define _GAME_H_

#include <cassert>
#include <vector>
#include <string>
#include <list>
#include <memory>
#include <istream>
#include "GameObject.h"
#include "json/json.h"


//forward declare classes to fix cyclic dependencies
class Action;
class BuildAction;


class SimulationException : public std::runtime_error {
public:
    SimulationException(const std::string &msg)
        : std::runtime_error(msg){};
};


class Game {
public:

    virtual ~Game(){};
    unsigned int getMineralAmount(){return minerals;}
    unsigned int getMineralMiningWorkers(){return mineralMiningWorkers;}
    unsigned int getGasAmount(){return gas;}
    unsigned int getGasMiningWorkers(){return gasMiningWorkers;}
    unsigned int getUsedSupplyAmount(){return usedSupply;}
    unsigned int getTotalSupplyAmount(){return totalSupply;}
    unsigned int getAvailableSupplyAmount(){return totalSupply-usedSupply;}
    unsigned int getCurrentTime(){return curTime;};

    void setMineralAmount(int amount){minerals = amount;}
    void setGasAmount(int amount){gas = amount;}
    void setUsedSupplyAmount(int amount){usedSupply = amount;}
    void setTotalSupplyAmount(int amount){totalSupply = amount;}

    void readConfiguration();

    void readBuildList(std::string filename);

    Json::Value& modifyOutput(){return output;}
    void printOutput();

    bool precheckBuildList();

    void setMuleAction(int value){muleActions += value;};

    void simulate();

    virtual Race getRace() = 0;
    std::string getRaceString(){
        switch(getRace()){
            case Race::PROTOSS:
                return "protoss";
            case Race::ZERG:
                return "zerg";
            case Race::TERRAN:
                return "terran";
            default:
                return "unknown";
        }
    }

protected:
    GameObject& mainBuilding;
    GameObject& worker;
    GameObject& geyserExploiter;
    std::list<std::shared_ptr<Action>> runningActions;

    Json::Value output;

    Game(GameObject& mainBuilding, GameObject& worker, GameObject& geyserExploiter);

    void debugOutput(std::shared_ptr<Action> action, bool start);
    virtual void invokeSpecial() = 0;

private:
    unsigned int curTime = 1;

    unsigned int minerals = 50*1000;
    unsigned int gas = 0;
    unsigned int usedSupply = 0;
    unsigned int totalSupply = 0;

    unsigned int gasMiningWorkers = 0;
    unsigned int mineralMiningWorkers = 6;

    unsigned int muleActions = 0;
    unsigned int freeWorkers = 6;

    unsigned int finishTimeCurrentBuildItem = 1;

    const int mineralsRate = DEFAULT_MINERAL_INCREASE;
    const int gasRate = DEFAULT_GAS_INCREASE;
    const int energyRate = DEFAULT_ENERGY_INCREASE;

    std::vector<std::shared_ptr<BuildAction>> buildList;

    std::vector<std::shared_ptr<BuildAction>>::iterator currBuildListItem;

    void readBuildList(std::istream &input);
    bool timeStep();
    void generateResources();
    bool finishBuildAction();
    void updateMessagesForWorkersReassignment();
    int ternarySearch(int left, int right, int neededGas,int neededMineral, int freeWorkers);
    int getMiningTime(int gasMiningWorkers, int mineralMiningWorkers, int neededGas,int neededMineral);
};


class ProtossGame : public Game {
public:
    ProtossGame();

    virtual Race getRace(){return Race::PROTOSS;};

protected:
    virtual void invokeSpecial() override;
};


class TerranGame : public Game {
public:
    TerranGame();

    virtual Race getRace(){return Race::TERRAN;};

protected:
    virtual void invokeSpecial() override;
};


class ZergGame : public Game {
public:
    ZergGame();

    virtual Race getRace(){return Race::ZERG;};

protected:
    virtual void invokeSpecial() override;

private:
    constexpr static unsigned int LARVA_SPAWN_TIME = 15;

    struct LarvaProducerProperties {
        unsigned int occupiedSlots = 0;
        unsigned int timeTillSpawn = LARVA_SPAWN_TIME;
    };

    GameObject& larva;
    std::vector<GameObject*> larvaProducerTypes;

    std::vector<LarvaProducerProperties> larvaProducerProperties;
    unsigned int previousLarvaCount = 0;
};

#endif
