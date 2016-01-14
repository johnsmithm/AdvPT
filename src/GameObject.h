#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include <functional>
#include <iostream>
#include <list>
#include <memory>
#include <string>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <vector>

#include "Config.h"

class GameObject;
class Game;


enum class BuildType {
    MORPH,
    ACTIVE_BUILD,
    INSTANTIATE
};


enum class Race {
    TERRAN,
    PROTOSS,
    ZERG
};


class TechTreeParsingException : public std::runtime_error {
public:
    TechTreeParsingException(const std::string &msg)
        : std::runtime_error(msg) {}

    TechTreeParsingException(const std::string &msg, int line)
        : std::runtime_error(msg + " on line " + std::to_string(line)) {}
};


class GameObjectInstance {
friend class GameObject;
public:
    GameObjectInstance(unsigned int energy, GameObject &type)
      : ID(maxID++), type(type), energy(energy), business(0),
        boostTarget (false) {};

    unsigned int getID() const {return ID;}
    GameObject& getType() const {return type;}

    bool hasEnergy(unsigned int value) const {return (value <= energy);}
    void updateEnergy(int val) {energy += val;}
    
    unsigned int getBusiness() const {return business;}
    void increaseBusiness() {++business;}
    void decreaseBusiness() {--business;}
    bool isBusy() const;
    void setBusy();
    
    bool isBoostTarget() const {return boostTarget;}
    void setBoostTarget(bool value) {boostTarget = value;}

    //TODO: ask at the chair, why the fuck?!
    bool operator==(const GameObjectInstance& other) const {return ID==other.ID;}

    friend std::ostream& operator<<(std::ostream &out, const GameObjectInstance &other);

private:
    static unsigned int maxID;

    const unsigned int ID;
    GameObject &type;

    unsigned int energy;
    unsigned int business;
    bool boostTarget;
};


class GameObject {
friend class GameObjectInstance;

public:
    GameObject(std::string name,
               unsigned int mineralCost, unsigned int gasCost,
               unsigned int buildTime,
               unsigned int supplyCost, unsigned int supplyProvided,
               unsigned int startEnergy, unsigned int maxEnergy,
               unsigned int maxBusyness,
               std::vector<std::string> producerNames,
               std::vector<std::string> dependencyNames,
               BuildType buildType,
               bool isBuilding)
        : name(name), mineralCost(mineralCost), gasCost(gasCost), buildTime(buildTime),
          supplyCost(supplyCost), supplyProvided(supplyProvided),
          startEnergy(startEnergy), maxEnergy(maxEnergy), maxBusyness(maxBusyness),
          producerNames(producerNames), dependencyNames(dependencyNames), buildType(buildType),
          building(isBuilding) {}

    GameObjectInstance& addNewInstance(Game &game);
    void removeInstance(const GameObjectInstance instance, Game &game);

    bool areDependenciesMet();
    GameObjectInstance* getPossibleProducer();
    
    unsigned int getInstancesCount() {return instances.size();}
    unsigned int getFreeInstancesCount();
    
    using InstancesIter = std::list<GameObjectInstance>::iterator;
    InstancesIter begin();
    InstancesIter end();

    unsigned int getMineralCost(){return mineralCost;}
    unsigned int getGasCost(){return gasCost;}
    unsigned int getSupplyCost(){return supplyCost;}
    unsigned int getBuildTime(){return buildTime;};
    BuildType getBuildType(){return buildType;};
    std::string getName(){return name;};
    const std::vector<std::string>& getProducerNames(){return producerNames;}
    const std::vector<std::string>& getDependencyNames(){return dependencyNames;}
    bool isBuilding(){return building;};

    static void parseFile(std::string filename);
    static void parseString(std::string input);
    static void parseStream(std::istream &inputStream);

    static GameObject& get(const std::string name);
    static std::vector<GameObjectInstance*> getAll(std::function<bool(GameObjectInstance&)> filter=[](GameObjectInstance &goi){return true;});

    static void increaseEnergy(int amount=DEFAULT_ENERGY_INCREASE);


private:
    std::string name;
    unsigned int mineralCost;
    unsigned int gasCost;
    unsigned int buildTime;
    unsigned int supplyCost;
    unsigned int supplyProvided;
    unsigned int startEnergy;
    unsigned int maxEnergy;
    unsigned int maxBusyness = 1;

    std::vector<std::string> producerNames;
    std::vector<std::string> dependencyNames;

    BuildType buildType;
    bool building;

    //unsigned int blockedInstaces;//will block Instances from left to right

    std::list<GameObjectInstance> instances;

    static std::unordered_map<std::string, std::shared_ptr<GameObject>> gameObjects;
};


#endif
