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

public:
    Action(Game& game)
        : game(game){};
    virtual bool canExecute() = 0;
    virtual void start() = 0;
    virtual void finish() = 0;
    virtual bool timeStep() = 0;
    virtual bool isBuildAction() {return false;}
    virtual std::string getName() {return "Generic Action";}
    Game& getGame() {return game;}
};


class BuildAction : public Action {
public:

    BuildAction(Game& game, GameObject& objectToBuild)
        : Action(game), objectToBuild(objectToBuild) {}

    virtual bool canExecute() override;
    virtual void start() override;
    virtual void finish() override;
    virtual bool timeStep() override;
    virtual bool isBuildAction() override {return true;}
    virtual std::string getName() override {return objectToBuild.getName();}

    unsigned int getGasCost() const {return objectToBuild.getGasCost();}
    unsigned int getSupplyCost() const  {return objectToBuild.getSupplyCost();}
    unsigned int getMineralCost() const {return objectToBuild.getMineralCost();}

    const std::string getName() const {return objectToBuild.getName();}
    GameObject& getObjectToBuild() {return objectToBuild;}
    int getProducerID() const;

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

    virtual bool canExecute() override {return true;}
    virtual void start() override;
    virtual void finish() override {target.setBoostTarget(false);}
    virtual bool timeStep() override {return --timeLeft == 0;};
    virtual std::string getName() override {return "BoostAction";}

    GameObjectInstance& getTarget() {return target;}
    GameObjectInstance& getSource() {return source;}

private:
    GameObjectInstance& target;
    GameObjectInstance& source;
    unsigned int timeLeft = BOOST_ACTION_TIME;
};


class MuleAction : public Action {
public:

    MuleAction(Game& game, GameObjectInstance& source)
        : Action(game), source(source) {}

    virtual bool canExecute() override {return true;}
    virtual void start() override;
    virtual void finish() override;
    virtual bool timeStep() override {return --timeLeft == 0;}
    virtual std::string getName() override {return "MuleAction";}

    GameObjectInstance& getSource() {return source;}

private:
    GameObjectInstance& source;
    unsigned int timeLeft = MULE_ACTION_TIME;
};


class QueenAction : public Action {
public:

    QueenAction(Game& game, GameObjectInstance& target)
        : Action(game), target(target) {}

    virtual bool canExecute() override {return true;}
    virtual void start() override;
    virtual void finish() override;
    virtual bool timeStep() override {return --timeLeft == 0;}
    virtual std::string getName() override {return "QueenAction";}

    GameObjectInstance& getTarget() {return target;}

private:
    GameObjectInstance& target;
    unsigned int timeLeft = QUEEN_EGGS_SPAWN_TIME;
};


#endif
