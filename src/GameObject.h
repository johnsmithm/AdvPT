#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include <functional>
#include <iostream>
#include <list>
#include <memory>
#include <mutex>
#include <string>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include "Const.h"


class GameObject;
class Game;


enum class BuildType {
    MORPH,
    PAIRMORPH,
    ACTIVE_BUILD,
    INSTANTIATE
};


enum class Race {
    TERRAN,
    PROTOSS,
    ZERG
};

Race raceFromString(std::string race);


class TechTreeParsingException : public std::runtime_error {
public:
    TechTreeParsingException(const std::string &msg)
        : std::runtime_error(msg) {}

    TechTreeParsingException(const std::string &msg, int line)
        : std::runtime_error(msg + " on line " + std::to_string(line)) {}
};


class GameObjectInstance {
public:
    GameObjectInstance(GameObject &type, unsigned int id, unsigned int energy, unsigned int productionLines)
      : id(id), type(&type), energy(energy), freeProductionLines(productionLines) {};

    unsigned int getID() const {return id;}
    GameObject& getType() {return *type;}
    void setType(GameObject& type) {this->type = &type;}

    unsigned int getEnergy() const {return energy;}
    void setEnergy(unsigned int value) {energy = value;}
    bool hasEnergy(unsigned int value) const {return (value <= energy);}
    void updateEnergy(int value) {energy += value;}

    unsigned int getFreeProductionLines() const {return freeProductionLines;}
    void setFreeProductionLines(unsigned int value) {freeProductionLines = value;}
    void increaseBusiness() {--freeProductionLines;}
    void decreaseBusiness() {++freeProductionLines;}

    unsigned int getOccupiedLarvaSlots() const {return occupiedLavaSlots;}
    void setOccupiedLarvaSlots(unsigned int value) {occupiedLavaSlots = value;}
    unsigned int getInjectedLarvaeEggs() const {return injectedLarvaEggs;}
    void setInjectedLarvaeEggs(unsigned int value) {injectedLarvaEggs = value;}
    unsigned int decreaseTimeTillLarvaSpawn() {return --timeTillLarvaSpawn;}
    void resetTimeTillLarvaSpawn() {timeTillLarvaSpawn = LARVA_SPAWN_TIME;}

    bool isBoostTarget() const {return boostTarget;}
    void setBoostTarget(bool value) {boostTarget = value;}

    bool isDead() {return dead;}
    void setDead(bool dead) {this->dead = dead;}

    bool operator==(const GameObjectInstance& other) const {return id == other.id;}

private:
    const unsigned int id;
    GameObject* type;

    unsigned int energy;
    unsigned int freeProductionLines;
    unsigned int occupiedLavaSlots = 0;
    unsigned int injectedLarvaEggs = 0;
    unsigned int timeTillLarvaSpawn = LARVA_SPAWN_TIME;
    bool boostTarget = false;
    bool dead = false;
};


class GameObject {
public:
    // Iterator type; allows to iterate through GameObjectInstances (see begin(), end())
    using InstancesIter = std::list<GameObjectInstance>::iterator;

    GameObject(std::string name,
               unsigned int mineralCost, unsigned int gasCost, unsigned int buildTime,
               unsigned int supplyCost, unsigned int supplyProvided, unsigned int startEnergy,
               unsigned int maxEnergy, unsigned int productionLines, Race race,
               std::vector<std::string> producerNames, std::vector<std::string> dependencyNames,
               BuildType buildType, bool isBuilding);

    const std::string getName() const {return name;}
    unsigned int getMineralCost() const {return mineralCost;}
    unsigned int getGasCost() const {return gasCost;}
    unsigned int getBuildTime() const {return buildTime;}
    unsigned int getSupplyCost() const {return supplyCost;}
    unsigned int getSupplyProvided() const {return supplyProvided;}
    unsigned int getProductionLines() const {return productionLines;}
    Race getRace(){return race;}
    const std::vector<std::string>& getProducerNames() const {return producerNames;}
    const std::vector<std::string>& getDependencyNames() const {return dependencyNames;}
    BuildType getBuildType() const {return buildType;}
    bool isBuilding() const {return building;}

    bool areDependenciesMet(int gameId) const;
    GameObjectInstance* getPossibleProducer(int gameId);

    unsigned int getInstancesCount(int gameId) {return instances(gameId).size();}
    unsigned int getFreeInstancesCount(int gameId);

    GameObjectInstance& addNewInstance(int gameId, Game& game);
    void morphInstance(int gameId, GameObjectInstance& source);

    InstancesIter begin(int gameId);
    InstancesIter end(int gameId);

    int getIntroductionProbability(){return introductionProbability;}
    int getDeletionProbability(){return deletionProbability;}

    void setIntroductionProbability(int probability) {introductionProbability = probability;}
    void setDeletionProbability(int probability) {deletionProbability = probability;}

    static void parseFile(std::string filename);
    static void parseString(std::string input);
    static void parseStream(std::istream &inputStream);

    static GameObject& get(const std::string name);
    static std::vector<GameObject*> getAll(std::function<bool(GameObject&)> filter=[](GameObject &go){return true;});
    static std::vector<GameObjectInstance*> getAllInstances(int gameId, std::function<bool(GameObjectInstance&)> filter=[](GameObjectInstance &goi){return true;});
    static void removeAllInstances(int gameId);

    static void increaseInstancesEnergy(int gameId, int value=DEFAULT_ENERGY_INCREASE);

    std::list<GameObjectInstance>& instances(int gameId){
        //lock for thread safety: this methods creates lists of instances, if needed
        std::lock_guard<std::mutex> lock(instancesCreationMutex);
        return instances_per_game[gameId];
    }


private:
    std::string name;
    unsigned int mineralCost;
    unsigned int gasCost;
    unsigned int buildTime;
    unsigned int supplyCost;
    unsigned int supplyProvided;
    unsigned int startEnergy;
    unsigned int maxEnergy;
    unsigned int productionLines;

    Race race;

    std::vector<std::string> producerNames;
    std::vector<std::string> dependencyNames;

    BuildType buildType;
    bool building;

    //probabilities between 0 and RAND_MAX
    int introductionProbability; //prob that this object will be introduced into buildlist on mutate
    int deletionProbability; //prob that this object will be deleted from buildlist on mutate

    std::unordered_map<int, std::list<GameObjectInstance>> instances_per_game;
    static std::mutex instancesCreationMutex;

    static std::unordered_map<std::string, GameObject> gameObjects;
};


#endif
