#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include <vector>
#include <string>

class GameObject;


enum class BuildType {
    MORPH,
    ACTIVE_BUILD,
    INSTANTIATE
};


class GameObjectInstance {
public:
    bool hasEnergy(unsigned int val);
    bool isBusy();
    void updateEnergy(int val);

private:
    static unsigned int maxID;//--???

    unsigned int ID;
    unsigned int energy;
    unsigned int business;
    bool muleTarget;//??

    GameObject* type;
};


class GameObject {
public:
    GameObject(std::string name,
               unsigned int mineralCost, unsigned int gasCost,
               unsigned int buildTime,
               unsigned int supplyCost, unsigned int supplyProvided,
               unsigned int startEnergy, unsigned int maxEnergy,
               unsigned int maxBusiness,
               std::vector<std::string> producers,
               std::vector<std::string> dependencies)
        : name(name), mineralCost(mineralCost), gasCost(gasCost), buildTime(buildTime),
          supplyCost(supplyCost), supplyProvided(supplyProvided),
          startEnergy(startEnergy), maxEnergy(maxEnergy), maxBusiness(maxBusiness),
          producers(producers), dependencies(dependencies) {}

    void addNewInstance();

    void removeInstance(GameObjectInstance instance);

private:
    std::string name;//how fast to find element by name
    unsigned int mineralCost;
    unsigned int gasCost;
    unsigned int buildTime;
    unsigned int supplyCost;
    unsigned int supplyProvided;
    //unsigned int larvaCost;//?? bad aproach
    unsigned int startEnergy;
    unsigned int maxEnergy;
    unsigned int maxBusiness;

    std::vector<std::string> producers;
    std::vector<std::string> dependencies;

    BuildType buildType;
    //unsigned int blockedInstaces;//will block Instances from left to right

    std::vector<GameObjectInstance> instances;

};


#endif
