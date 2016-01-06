#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

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
        : std::runtime_error(msg){};

    TechTreeParsingException(const std::string &msg, int line)
        : std::runtime_error(msg + " on line " + std::to_string(line)){};
};


class GameObjectInstance {
friend class GameObject;
public:
    GameObjectInstance(unsigned int energy, GameObject &type)
      : ID(maxID++), energy(energy), business(0),
        boostTarget (false), type(type) {};

    void decreaseBusiness();
    bool hasEnergy(unsigned int val);
    bool isBoostTarget(){return boostTarget;};

    bool isBusy();
    void increaseEnergy();
    void increaseBusiness();
    void updateEnergy(int val);

    void setBoostTarget(bool isBoostTarget){
        boostTarget = isBoostTarget;
    }

    //TODO: ask at the chair, why the fuck?!
    bool operator==(const GameObjectInstance &other){
        return ID==other.ID;
    }

private:
    static unsigned int maxID;

    const unsigned int ID;
    unsigned int energy;
    unsigned int business = 0;
    bool boostTarget;

    GameObject &type;
};

class GameObject {
friend class GameObjectInstance;

public:
    GameObject(std::string name,
               unsigned int mineralCost, unsigned int gasCost,
               unsigned int buildTime,
               unsigned int supplyCost, unsigned int supplyProvided,
               unsigned int startEnergy, unsigned int maxEnergy,
               unsigned int maxBusiness,
               std::vector<std::string> producerNames,
               std::vector<std::string> dependencyNames,
               BuildType buildType)
        : name(name), mineralCost(mineralCost), gasCost(gasCost), buildTime(buildTime),
          supplyCost(supplyCost), supplyProvided(supplyProvided),
          startEnergy(startEnergy), maxEnergy(maxEnergy), maxBusiness(maxBusiness),
          producerNames(producerNames), dependencyNames(dependencyNames), buildType(buildType) {}

    void addNewInstance(Game &game);
    void removeInstance(const GameObjectInstance instance, Game &game);

    void resolveNames();

    bool areDependenciesMet();
    GameObjectInstance* getPossibleProducer();
    unsigned int getFreeInstancesCount();
    unsigned int getInstancesCount(){return instances.size();}

    unsigned int getMineralCost(){return mineralCost;}
    unsigned int getGasCost(){return gasCost;}
    unsigned int getSupplyCost(){return supplyCost;}
    unsigned int getBuildTime(){return buildTime;};
    BuildType getBuildType(){return buildType;};
    std::string getName(){return name;};
    const std::vector<std::string>& getProducerNames(){return producerNames;}
    const std::vector<std::string>& getDependencyNames(){return dependencyNames;}

    static void parseFile(std::string filename);
    static void parseString(std::string input);
    static void parseStream(std::istream &inputStream);
    static GameObject& get(const std::string name);
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
    unsigned int maxBusiness = 1;

    std::vector<std::string> producerNames;
    std::vector<std::string> dependencyNames;

    BuildType buildType;
    //unsigned int blockedInstaces;//will block Instances from left to right

    std::list<GameObjectInstance> instances;

    static std::unordered_map<std::string, std::shared_ptr<GameObject>> gameObjects;
};


#endif
