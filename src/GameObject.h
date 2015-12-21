#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include <vector>

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
    void addNewInstance();
    
    void removeInstance(GameObjectInstance instance);

private:
    unsigned int mineralCost;
    unsigned int gasCost;
    unsigned int buildTime;//
    unsigned int suplyCost;//
    unsigned int larvaCost;//?? bad aproach
    unsigned int maxBusiness;
    unsigned int maxEnergy;

    std::string name;//how fast to find element by name

    unsigned int blockedInstaces;//will block Instances from left to right

    std::vector<GameObject> possibleProducers;
    std::vector<GameObject> dependencies;

    BuildType buildType;

    std::vector<GameObjectInstance> instances;

};


#endif
