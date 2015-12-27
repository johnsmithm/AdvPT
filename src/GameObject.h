#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include <memory>
#include <string>
#include <vector>

class GameObject;

#include "TechTree.h"

enum class BuildType {
    MORPH,
    ACTIVE_BUILD,
    INSTANTIATE
};


class GameObjectInstance {
public:
    GameObjectInstance(unsigned int energy, GameObject &type)
      : ID(maxID++), energy(energy), business(0), type(type){};

    bool hasEnergy(unsigned int val);
    bool isBusy();
    void updateEnergy(int val);

private:
    static unsigned int maxID;

    const unsigned int ID;
    unsigned int energy;
    unsigned int business;
    bool muleTarget;//??

    GameObject &type;
};


class GameObject {
public:
    GameObject(std::string name,
               unsigned int mineralCost, unsigned int gasCost,
               unsigned int buildTime,
               unsigned int supplyCost, unsigned int supplyProvided,
               unsigned int startEnergy, unsigned int maxEnergy,
               unsigned int maxBusiness,
               std::vector<std::string> producerNames,
               std::vector<std::string> dependencyNames)
        : name(name), mineralCost(mineralCost), gasCost(gasCost), buildTime(buildTime),
          supplyCost(supplyCost), supplyProvided(supplyProvided),
          startEnergy(startEnergy), maxEnergy(maxEnergy), maxBusiness(maxBusiness),
          producerNames(producerNames), dependencyNames(dependencyNames) {}

    void addNewInstance();
    void removeInstance(GameObjectInstance instance);

    void resolveNames(TechTree tt);

    std::string name;//how fast to find element by name
private:
    unsigned int mineralCost;
    unsigned int gasCost;
    unsigned int buildTime;
    unsigned int supplyCost;
    unsigned int supplyProvided;
    //unsigned int larvaCost;//?? bad aproach
    unsigned int startEnergy;
    unsigned int maxEnergy;
    unsigned int maxBusiness;

    std::vector<std::string> producerNames;
    std::vector<std::string> dependencyNames;

    std::vector<std::shared_ptr<GameObject>> producers = {nullptr}; //producers[0]!=nullptr can be asserted
    std::vector<std::shared_ptr<GameObject>> dependencies = {nullptr};

    BuildType buildType;
    //unsigned int blockedInstaces;//will block Instances from left to right

    std::vector<GameObjectInstance> instances;

};


#endif
