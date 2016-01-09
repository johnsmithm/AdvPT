#ifndef _ACTION_H_
#define _ACTION_H_

#include <memory>

//forward declare classes to fix cyclic dependencies
class Game;

#include "Game.h"
#include "GameObject.h"


class Action {
protected:
    Game& game;
    unsigned int finishTime;

public:
    Action(Game& game)
        : game(game){};
    virtual bool canExecute() = 0;
    virtual void start() = 0;
    virtual void finish() = 0;
    virtual bool timeStep() = 0;
    virtual std::string getName() = 0;
};


class BuildAction : public Action {
public:

    BuildAction(Game& game, GameObject& objectToBuild)
        : Action(game), objectToBuild(objectToBuild) {}

    bool canExecute();

    void start();

    void finish();

    bool timeStep();

    unsigned int getGasCost(){return objectToBuild.getGasCost();};
    unsigned int getSupplyCost(){return objectToBuild.getSupplyCost();};
    unsigned int getMineralCost(){return objectToBuild.getMineralCost();};

    std::string getName(){return objectToBuild.getName();};
    GameObject& getObjectToBuild(){return objectToBuild;};

private:
    GameObject& objectToBuild;
    GameObjectInstance* producingInstance = nullptr;

    int timeLeft;
};


/*class BoostAction {//can a building be target again?
private:
    GameObjectInstance &target;
    void start(){
         //check Target Actions, change finishing time
        CalculateFinishingTime();
    }

    void finish(){
       target.unMarck();
    }
};


class CreateLarvaeHatcharyAction {//from eggs or from hatcharies??
private:
    GameObjectInstance &Hatchary;
    void finish(){
       ++NrLarvaue;
    }
}


class CreateLarvaeEggsAction {//from eggs or from hatcharies??
private:
    GameObjectInstance &Hatchary;
    void finish(){
       NrLarvaue += 4;
    }
}


class MuleAction {
private:
   GameObject& commandCenter;
   GameObject& orbitalCommand;
   GameObjectInstance &target;
public:

    MuleAction(GameObject& commandCenter, GameObject& orbitalCommand)
        commandCenter(commandCenter), orbitalCommand(orbitalCommand){}//can be inatializated at the begining

    bool canExecute(){//can be executed more time per second???
        for(GameObjectInstance command : commandCenter.instanceID)
                 if(Command.hasEnergy()){
                    target = command;
                    return true;
                 }
        //same for orbital
    }
    void start(){
     target.UpdateEnergy(-50*1000);
     MuleIncrese = MineralRate * 4;
     CalculateFinishingTime();
    }
    void finish(){
     MuleIncrese = 0;
    }
};*/


#endif
