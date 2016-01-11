#ifndef _ACTION_H_
#define _ACTION_H_

#include <memory>

//forward declare classes to fix cyclic dependencies
class Game;

#include "Game.h"
#include "GameObject.h"
#include "json/json.h"


class Action {
protected:
    Game& game;
    unsigned int finishTime;

    Json::Value& updateMessage();

public:
    Action(Game& game)
        : game(game){};
    virtual bool canExecute() = 0;
    virtual void start() = 0;
    virtual void finish() = 0;
    virtual bool timeStep() = 0;
    virtual std::string getName(){return "Generic Action";};
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


class BoostAction : public Action{
public:
    BoostAction(Game& game, GameObjectInstance& target, GameObjectInstance& source)
        : Action(game), target(target), source(source) {}

    virtual bool canExecute(){return true;};
    virtual void start();
    virtual void finish(){target.setBoostTarget(false);}
    virtual bool timeStep(){return --timeLeft == 0;};
    virtual std::string getName(){return "BoostAction";};

private:
    GameObjectInstance &target;
    GameObjectInstance &source;
    unsigned int timeLeft = 20;
};

/*
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
