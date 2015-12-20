#include <vector>
#include <string>

#include "GameObject.h"
#include "Action.h"

class Game {
private:
    int curTime = 0;

    int minerals = 0;
    int gas = 0;
    int supply = 0;
    int larva = 0;

    int mineralsRate = 0;
    int gasRate = 0;
    int energyRate = 0;

    int mineralsIncrise = 0;
    int gasIncrise = 0;
    int MuleIncrese = 0;

    int exploitedGeysers = 0;
    int freeWorkers = 0;
    int mineralMiningWorkers = 0;
    int gasMiningWorkers = 0;

    std::vector<Action> runningActions;
    std::vector<Action> buildList;
    std::vector<GameObject> TechTree;
    
    Action curBuildListItem; //vector iterator
    //GameObject * BuildingWithEnergy;
    Race curRace;

    std::string Output;


public:

    enum Race {
        TERRAN,
        PROTOS,
        ZERG
    }

    Race getRace()const {
        return curRace;
    }

    void timeStep() {
        if(curBuildListItem == buildList.end())stop();
        
        //Add Resurces from previous second 
        minerals += mineralsIncrise;
        gas += gasIncrise;
        
        //increse energy
        EnergyIncrease();
        
        //check runningAction
        for(auto item : runningActions)
            if(item.isFinished){
                item.finish();
                item.erase();
            }else{
                item.updateTime();
            }
        
        //check buildList
        if(curBuildListItem.canExecute()){
          curBuildListItem.start();
          runningActions.push_back(curBuildListItem);   
          curBuildListItem++;    
        }
        
        workersAsignment();
        
        //write messages
        report();
        
        ++curTime;
    }
    
    void readConfiguration(){
        //read line
        //create GameObject
    }
    void readBuildList(){
       //read name of item to build
       //find GameObject from techtree
       //construct Action    
    }
    
    void printOutput(){
    //print String
    }

    void initialization() {//can be done in constructor
        curBuildListItem = buildList[0];
        mineralsRate = 10;
        gasRate = 10;
    }
    
    virtual void EnergyIncrese() = 0;//{ for()item.increse() }??
    
    void workersAsignment() {//naive
        if (!needGas(curBuildListItem)) {
            //All workers harv minerals
            mineralMiningWorkers += freeWorkers + gasMiningWorkers;
        } else {
            //Maximum numbers of workers go to gas, others go to minerals
            freeWorkers += mineralMiningWorkers + gasMiningWorkers;
            if (exploitedGeysers * 3 <= freeWorkers) {
                //we have more workers than Geysers need
                gasMiningWorkers = exploitedGeysers * 3;
                freeWorkers -= gasMiningWorkers;
                mineralMiningWorkers = freeWorkers;
            } else {
                gasMiningWorkers = freeWorkers;
                mineralMiningWorkers = 0;
            }
        }
        freeWorkers = 0;
        mineralsIncrise = mineralMiningWorkers * mineralsRate + MuleIncrese;
        gasIncrise = gasMiningWorkers * gasRate;
    }
};

class ZergGame : Game {
    Race curRace = ZERG;
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
    EnergyIncrese(){
         for(GameObjectInstace Queen : QueenType.instanceID)    
                Queen.increseEnergy(EnergyRate);
    }
};

class ProtosGame : Game {
    Race curRace = PROTOS;   
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
    EnergyIncrese(){
      for(GameObjectInstace Nexus : NexusType.instanceID)
            Nexus.increseEnergy(EnergyRate);
    }
};

class TerranGame : Game {
    Race curRace = TERRAN; 
    std::vector<GameObject& > EnergyBuildings; //OrbitaCommand , CenterCommand
    void workersAsignment() {
         super.workersAsignment();
         for(GameObject Building : EnergyBuildings)//can be done with actions
          for(GameObjectInstace Command : Building.instanceID)
                 if(Command.hasEnergy()){
                    CreateActionMULE();
                    Command.updateEnergy(-MULE.getEnergy());//set manually
                    MuleIncrese = 4*mineralRate;
                 }
    }
    EnergyIncrese(){
        for(GameObject Building : EnergyBuildings)
          for(GameObjectInstace Command : Building.instanceID)
                Command.increseEnergy(EnergyRate);
    }
};
