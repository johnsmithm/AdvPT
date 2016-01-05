#ifndef _GAME_H_
#define _GAME_H_

#include <cassert>
#include <vector>
#include <string>
#include <list>
#include <memory>
#include <istream>

#include "GameObject.h"
#include "Action.h"

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
    Game(GameObject& worker, GameObject& geyserExploiter);

    unsigned int getMineralAmount(){return minerals;}
    unsigned int getGasAmount(){return gas;}
    unsigned int getUsedSupplyAmount(){return usedSupply;}
    unsigned int getTotalSupplyAmount(){return totalSupply;}
    unsigned int getAvailableSupplyAmount(){return totalSupply-usedSupply;}

    void setMineralAmount(int amount){minerals = amount;}
    void setGasAmount(int amount){gas = amount;}
    void setUsedSupplyAmount(int amount){usedSupply = amount;}
    void setTotalSupplyAmount(int amount){totalSupply = amount;};

    void readConfiguration();

    void readBuildList(std::string filename);

    void printOutput();
    
    

    void simulate();

protected:
    GameObject& worker;
    GameObject& geyserExploiter;

private:
    int curTime = 1;

    unsigned int minerals = 0;
    unsigned int gas = 0;
    unsigned int usedSupply = 0;
    unsigned int totalSupply = 0;
    
    unsigned int gasMiningWorkers = 0;
    unsigned int  mineralMiningWorkers = 6;

    const int mineralsRate = DEFAULT_MINERAL_INCREASE;
    const int gasRate = DEFAULT_GAS_INCREASE;
    const int energyRate = DEFAULT_ENERGY_INCREASE;

    unsigned int exploitedGeysers = 0;

    std::list<std::shared_ptr<Action>> runningActions;
    std::vector<std::shared_ptr<BuildAction>> buildList;

    std::vector<std::shared_ptr<BuildAction>>::iterator currBuildListItem;

    void readBuildList(std::istream &input);
    void writeMessages(std::shared_ptr<Action> action, bool start);
    bool timeStep();
    void generateResources();

    std::string output = "";
};


class ProtosGame : public Game {
public:
    ProtosGame();

private:

};


/*class ZergGame : Game {
    Race currRace = ZERG;
    GameObject & QueenType;
    GameObject & HatcheryType;
    void workersAsignment() {
        super.workersAsignment();
        for(GameObjectInstace Queen : QueenType.instanceID)//can be done with actions
            for(GameObjectInstace Hatchery : HatcheryType)
                if(Queen.hasEnergy() && !Hatchery.isFull()){
                    CreateActionLarvaeEggs();
                    Queen.updateEnergy(-somefixedval);
                    Hatchery.UpdateEggNumber();
                }
    }
    increaseEnergy(){
         for(GameObjectInstace Queen : QueenType.instanceID)
                Queen.increseEnergy(EnergyRate);
    }
};

class ProtosGame : Game {
    Race currRace = PROTOS;
    GameObject & NexusType;
    void workersAsignment() {
        super.workersAsignment();
        for(GameObjectInstace Nexus : NexusType.instanceID)//can be done with actions
            if(Nexus.hasEnergy() && !runningBuildingActions.empty()){
                CreateActionCronoBoost(MaxTimeBuildingActions.producer());
                Nexus.updateEnergy(-Queen.getEggEnergyCost());//set manually
                MaxTimeBuildingActions.producer.Actions.UpdateActionFinisingTime();
                MaxTimeBuildingActions.producer.mark();
            }
    }
    increaseEnergy() {
        for(GameObjectInstace Nexus : NexusType.instanceID)
            Nexus.increseEnergy(EnergyRate);
    }
};

class TerranGame : Game {
    Race currRace = TERRAN;
    std::vector<GameObject& > EnergyBuildings; //OrbitaCommand , CenterCommand
    void workersAsignment() {
        super.workersAsignment();
        for (GameObject Building : EnergyBuildings)//can be done with actions
            for (GameObjectInstace Command : Building.instanceID)
                if(Command.hasEnergy()) {
                    CreateActionMULE();
                    Command.updateEnergy(-MULE.getEnergy());//set manually
                    MuleIncrese = 4*mineralRate;
                }
    }
    increaseEnergy() {
        for (GameObject Building : EnergyBuildings)
            for (GameObjectInstace Command : Building.instanceID)
                Command.increseEnergy(EnergyRate);
    }
};*/


#endif
