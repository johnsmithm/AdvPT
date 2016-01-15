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
public:
    GameObjectInstance(GameObject &type, unsigned int energy, unsigned int productionLines)
      : ID(maxID++), type(type), energy(energy), freeProductionLines(productionLines),
        dead(false), boostTarget (false) {};

    unsigned int getID() const {return ID;}
    GameObject& getType() const {return type;}

    unsigned int getEnergy() const {return energy;}
    void setEnergy(unsigned int value) {energy = value;}
    bool hasEnergy(unsigned int value) const {return (value <= energy);}
    void updateEnergy(int value) {energy += value;}
    
    unsigned int getFreeProductionLines() const {return freeProductionLines;}
    void increaseBusiness() {--freeProductionLines;}
    void decreaseBusiness() {++freeProductionLines;}

    bool isDead() {return dead;}
    void setDead() {dead = true;}

    bool isBoostTarget() const {return boostTarget;}
    void setBoostTarget(bool value) {boostTarget = value;}

    //TODO: ask at the chair, why the fuck?!
    bool operator==(const GameObjectInstance& other) const {return ID==other.ID;}

private:
    static unsigned int maxID;

    const unsigned int ID;
    GameObject& type;

    unsigned int energy;
    unsigned int freeProductionLines;
    bool dead;
    bool boostTarget;
};


class GameObject {
public:
    GameObject(std::string name,
               unsigned int mineralCost, unsigned int gasCost, unsigned int buildTime,
               unsigned int supplyCost, unsigned int supplyProvided, unsigned int startEnergy,
               unsigned int maxEnergy, unsigned int productionLines,
               std::vector<std::string> producerNames, std::vector<std::string> dependencyNames,
               BuildType buildType, bool isBuilding);

    unsigned int getMineralCost() const {return mineralCost;}
    unsigned int getGasCost() const {return gasCost;}
    unsigned int getSupplyCost() const {return supplyCost;}
    unsigned int getBuildTime() const {return buildTime;}
    unsigned int getProductionLines() const {return productionLines;}
    BuildType getBuildType() const {return buildType;}
    std::string getName() const {return name;}
    const std::vector<std::string>& getProducerNames() const {return producerNames;}
    const std::vector<std::string>& getDependencyNames() const {return dependencyNames;}
    bool isBuilding() const {return building;}

    bool areDependenciesMet();
    GameObjectInstance* getPossibleProducer();

    unsigned int getInstancesCount() {return instances.size();}
    unsigned int getFreeInstancesCount();

    GameObjectInstance& addNewInstance(Game &game);
    void removeInstance(const GameObjectInstance instance, Game &game);

    using InstancesIter = std::list<GameObjectInstance>::iterator;
    InstancesIter begin();
    InstancesIter end();

    static void parseFile(std::string filename);
    static void parseString(std::string input);
    static void parseStream(std::istream &inputStream);

    static GameObject& get(const std::string name);
    static std::vector<GameObjectInstance*> getAll(std::function<bool(GameObjectInstance&)> filter=[](GameObjectInstance &goi){return true;});

    static void increaseInstancesEnergy(int value=DEFAULT_ENERGY_INCREASE);

private:
    std::string name;
    unsigned int mineralCost;
    unsigned int gasCost;
    unsigned int buildTime;
    unsigned int supplyCost;
    unsigned int supplyProvided;
    unsigned int startEnergy;
    unsigned int maxEnergy;
    unsigned int productionLines = 1;

    std::vector<std::string> producerNames;
    std::vector<std::string> dependencyNames;

    BuildType buildType;
    bool building;

    std::list<GameObjectInstance> instances;

    static std::unordered_map<std::string, std::shared_ptr<GameObject>> gameObjects;
};


#endif
