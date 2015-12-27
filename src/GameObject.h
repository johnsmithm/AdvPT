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
    bool hasEnergy(unsigned int val){
        return (val <= energy);
    }

    void updateEnergy(int val){ // val can be positive or negative
        energy += val;
    }

    bool isBusy() {
        return (business == 0);
    }

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
    GameObject(unsigned int mineralCost, unsigned int gasCost,
               unsigned int buildTime, unsigned int supplyCost,
               unsigned int supplyProvided, unsigned int maxEnergy,
               unsigned int maxBusiness, BuildType buildType)
        : mineralCost(mineralCost), gasCost(gasCost), buildTime(buildTime),
          supplyCost(supplyCost), supplyProvided(supplyProvided),
          maxEnergy(maxEnergy), maxBusiness(maxBusiness), buildType(buildType) {}

    void addNewInstance();

    void removeInstance(GameObjectInstance instance);

private:
    std::string name;//how fast to find element by name
    unsigned int mineralCost;
    unsigned int gasCost;
    unsigned int buildTime;//
    unsigned int supplyCost;//
    unsigned int supplyProvided;
    //unsigned int larvaCost;//?? bad aproach
    unsigned int maxEnergy;
    unsigned int maxBusiness;
    std::vector<GameObject*> possibleProducers;
    std::vector<GameObject*> possibleDependencies;
    BuildType buildType;
    //unsigned int blockedInstaces;//will block Instances from left to right

    std::vector<GameObjectInstance> instances;

};


#endif
