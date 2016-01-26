#ifndef _ACTION_H_
#define _ACTION_H_

#include <memory>
#include "Game.h"
#include "GameObject.h"
#include "json/json.h"


class Action {
public:
    virtual void finish() = 0;
    virtual bool timeStep() = 0;
    virtual bool isBuildAction() {return false;}
    virtual std::string getName() {return "Generic Action";}
    virtual Game& getGame() = 0;

protected:
    unsigned int finishTime;
};


class BuildAction : public Action {
public:
    BuildAction(Game& game, GameObject& objectToBuild)
        : game(game), objectToBuild(objectToBuild) {}

    bool tryToStart();
    virtual void finish() override;
    virtual bool timeStep() override;
    virtual bool isBuildAction() override {return true;}
    virtual std::string getName() override {return objectToBuild.getName();}
    virtual Game& getGame() {return game;}

    unsigned int getGasCost() const {return objectToBuild.getGasCost();}
    unsigned int getSupplyCost() const  {return objectToBuild.getSupplyCost();}
    unsigned int getMineralCost() const {return objectToBuild.getMineralCost();}

    const std::string getName() const {return objectToBuild.getName();}
    GameObject& getObjectToBuild() {return objectToBuild;}
    int getProducerID() const;

private:
    Game& game;
    int producerID;
    GameObject& objectToBuild;
    GameObjectInstance* producingInstance = nullptr;

    int timeLeft;
};


class BoostAction : public Action{
public:
    BoostAction(ProtossGame& game, GameObjectInstance& target, GameObjectInstance& source)
        : game(game), target(target), source(source) {}

    void start();
    virtual void finish() override {target.setBoostTarget(false);}
    virtual bool timeStep() override {return --timeLeft == 0;};
    virtual std::string getName() override {return "BoostAction";}
    virtual Game& getGame() {return game;}

    GameObjectInstance& getTarget() {return target;}
    GameObjectInstance& getSource() {return source;}

private:
    ProtossGame& game;
    GameObjectInstance& target;
    GameObjectInstance& source;
    unsigned int timeLeft = BOOST_ACTION_TIME;
};


class MuleAction : public Action {
public:

    MuleAction(TerranGame& game, GameObjectInstance& source)
        : game(game), source(source) {}

    void start();
    virtual void finish() override;
    virtual bool timeStep() override {return --timeLeft == 0;}
    virtual std::string getName() override {return "MuleAction";}
    virtual Game& getGame() {return game;}

    GameObjectInstance& getSource() {return source;}

private:
    TerranGame& game;
    GameObjectInstance& source;
    unsigned int timeLeft = MULE_ACTION_TIME;
};


class QueenAction : public Action {
public:

    QueenAction(ZergGame& game, GameObjectInstance& source, GameObjectInstance& target)
        : game(game), source(source), target(target) {}

    void start();
    virtual void finish() override;
    virtual bool timeStep() override {return --timeLeft == 0;}
    virtual std::string getName() override {return "QueenAction";}
    virtual Game& getGame() {return game;}

    GameObjectInstance& getSource() {return source;}
    GameObjectInstance& getTarget() {return target;}

private:
    ZergGame& game;
    GameObjectInstance& source;
    GameObjectInstance& target;
    unsigned int timeLeft = QUEEN_EGGS_SPAWN_TIME;
};


#endif
