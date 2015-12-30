#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include <memory>
#include <string>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <vector>

class GameObject;

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
public:
    GameObjectInstance(unsigned int energy, GameObject &type)
      : ID(++maxID), energy(energy), business(0), type(type){};

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
friend class GameObjectInstance;

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

    void resolveNames();

    bool areDependenciesMet();
    bool getPossibleProducer(GameObjectInstance* result);

    bool getMineralCost(){return mineralCost;}
    bool getGasCost(){return gasCost;}
    bool getSupplyCost(){return supplyCost;}

    static void parseFile(std::string filename);
    static void parseString(std::string input);
    static void parseStream(std::istream &inputStream);
    static std::shared_ptr<GameObject>& getGameObject(const std::string name);

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

    std::vector<std::string> producerNames;
    std::vector<std::string> dependencyNames;

    BuildType buildType;
    //unsigned int blockedInstaces;//will block Instances from left to right

    std::vector<GameObjectInstance> instances;

    static std::unordered_map<std::string, std::shared_ptr<GameObject>> gameObjects;
};


#endif
