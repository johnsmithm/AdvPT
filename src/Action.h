#ifndef _ACTION_H_
#define _ACTION_H_

#include <memory>
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
    int producerID;
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

class MuleAction : public Action {
public:

    MuleAction(Game& game, GameObjectInstance& source)
        : Action(game), source(source) {}

    virtual bool canExecute(){return true;};
    virtual void start();
    virtual void finish();
    virtual bool timeStep(){return --timeLeft == 0;};
    virtual std::string getName(){return "MuleAction";};

private:
    GameObjectInstance &source;
    unsigned int timeLeft = 90;
};


#endif
