#ifndef _ACTION_H_
#define _ACTION_H_

#include <memory>

#include "Game.h"
#include "GameObject.h"

//forward declare classes to fix cyclic dependencies
class Game;

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
};


class BuildAction : public Action {
private:
    std::shared_ptr<GameObject>& objectToBuild;
    GameObjectInstance* producingInstance;
public:

    BuildAction(Game& game, std::shared_ptr<GameObject>& objectToBuild)
        : Action(game), objectToBuild(objectToBuild), producingInstance(nullptr) {}

    bool canExecute();

    void start(){};

    void finish(){};
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
