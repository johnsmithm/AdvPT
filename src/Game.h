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

    Game(){
        currRace = Race::PROTOSS;
        GameObject &go = *GameObject::getGameObject("probe");
        go.addNewInstance(*this);
        go.addNewInstance(*this);
        go.addNewInstance(*this);

        go.addNewInstance(*this);
        go.addNewInstance(*this);
        go.addNewInstance(*this);

        GameObject::getGameObject("nexus")->addNewInstance(*this);
        setMineralAmount(50*10000);
    };

    Race getRace() const {
        return currRace;
    }

    unsigned int getMineralAmount(){return minerals;}
    unsigned int getGasAmount(){return gas;}
    unsigned int getSupplyAmount(){return supply;}

    void setMineralAmount(int amount){minerals = amount;}
    void setGasAmount(int amount){gas = amount;}
    void setSupplyAmount(int amount){supply = amount;}
    void setTotalSupplyAmount(int amount){totalSupply += amount;};

    void readConfiguration();

    void readBuildList(std::string filename);

    void printOutput();
    
    

    void simulate();


private:
    int curTime = 1;

    unsigned int minerals = 0;
    unsigned int gas = 0;
    unsigned int supply = 0;
    unsigned int totalSupply = 0;
    
    unsigned int gasMiningWorkers = 0;
    unsigned int  mineralMiningWorkers = 6;

    const int mineralsRate = DEFAULT_MINERAL_INCREASE;
    const int gasRate = DEFAULT_GAS_INCREASE;
    const int energyRate = DEFAULT_ENERGY_INCREASE;

    unsigned int exploitedGeysers = 0;

    std::list<std::shared_ptr<Action>> runningActions;
    std::vector<std::shared_ptr<BuildAction>> buildList;

    std::vector<std::shared_ptr<BuildAction>>::iterator currBuildListItem; //vector iterator
    //GameObject * BuildingWithEnergy;
    Race currRace;

    void readBuildList(std::istream &input);
    void writeMessages(std::shared_ptr<Action> action, bool start);
    bool timeStep();
    void generateResources();

    std::string output = "";
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
