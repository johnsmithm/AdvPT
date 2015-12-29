#ifndef _GAME_H_
#define _GAME_H_

#include <vector>
#include <string>
#include <memory>
#include <istream>
#include "GameObject.h"
#include "Action.h"

class SimulationException : public std::runtime_error {
public:
    SimulationException(const std::string &msg)
        : std::runtime_error(msg){};
};


class Game {
public:

    Game();

    Race getRace() const {
        return currRace;
    }

    void timeStep();

    void readConfiguration();

    void readBuildList(std::string filename);

    void printOutput();

    //virtual void increaseEnergy() = 0;//{ for()item.increse() }??

    void assignWorkers();

private:
    int curTime = 0;

    int minerals = 0;
    int gas = 0;
    int supply = 0;
    int larva = 0;

    int mineralsRate = 0;
    int gasRate = 0;
    int energyRate = 0;

    int mineralsIncrease = 0;
    int gasIncrease = 0;
    int MuleIncrease = 0;

    int exploitedGeysers = 0;
    int freeWorkers = 0;
    int mineralMiningWorkers = 0;
    int gasMiningWorkers = 0;

    std::vector<std::shared_ptr<Action>> runningActions;
    std::vector<std::shared_ptr<BuildAction>> buildList;

    BuildAction* currBuildListItem; //vector iterator
    //GameObject * BuildingWithEnergy;
    Race currRace;

    void readBuildList(std::istream &input);

    //std::string Output;
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
