#ifndef _ACTION_H_
#define _ACTION_H_

#include <memory>
#include "Game.h"
#include "GameObject.h"
#include "json/json.h"


class Action {
public:

    virtual std::string getName() const = 0;

};


class BuildAction : public Action {
public:
    BuildAction(Game& game, GameObject& objectToBuild)
        : game(game), objectToBuild(objectToBuild) {}

    bool tryToStart();

    void finish();
    bool timeStep();
    std::string getName() {return objectToBuild.getName();}
    Game& getGame() {return game;}


    unsigned int getGasCost() const {return objectToBuild.getGasCost();}
    unsigned int getSupplyCost() const  {return objectToBuild.getSupplyCost();}
    unsigned int getMineralCost() const {return objectToBuild.getMineralCost();}

    virtual std::string getName() const override {return objectToBuild.getName();}
    GameObject& getObjectToBuild() {return objectToBuild;}
    int getProducerID() const;

private:
    Game& game;
    GameObject& objectToBuild;
    GameObjectInstance* producingInstance = nullptr;

    int timeLeft;
};


class SpecialAction : public Action {
public:
    virtual void finish() = 0;
    virtual bool timeStep() = 0;
    virtual std::string getName() const {return "Generic SpecialAction";}
    virtual Game& getGame() = 0;

protected:
    unsigned int finishTime;
};


class BoostAction : public SpecialAction {
public:
    BoostAction(ProtossGame& game, GameObjectInstance& target, GameObjectInstance& source)
        : game(game), target(target), source(source) {}

    void start();
    virtual void finish() override {target.setBoostTarget(false);}
    virtual bool timeStep() override {return --timeLeft == 0;};
    virtual std::string getName() const override {return "BoostAction";}
    virtual Game& getGame() {return game;}

    GameObjectInstance& getTarget() {return target;}
    GameObjectInstance& getSource() {return source;}

private:
    ProtossGame& game;
    GameObjectInstance& target;
    GameObjectInstance& source;
    unsigned int timeLeft = BOOST_ACTION_TIME;
};


class MuleAction : public SpecialAction {
public:

    MuleAction(TerranGame& game, GameObjectInstance& source)
        : game(game), source(source) {}

    void start();
    virtual void finish() override;
    virtual bool timeStep() override {return --timeLeft == 0;}
    virtual std::string getName() const override {return "MuleAction";}
    virtual Game& getGame() {return game;}

    GameObjectInstance& getSource() {return source;}

private:
    TerranGame& game;
    GameObjectInstance& source;
    unsigned int timeLeft = MULE_ACTION_TIME;
};


class QueenAction : public SpecialAction {
public:

    QueenAction(ZergGame& game, GameObjectInstance& source, GameObjectInstance& target)
        : game(game), source(source), target(target) {}

    void start();
    virtual void finish() override;
    virtual bool timeStep() override {return --timeLeft == 0;}
    virtual std::string getName() const override {return "QueenAction";}
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
