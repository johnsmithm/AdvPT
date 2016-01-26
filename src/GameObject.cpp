#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cassert>
#include "GameObject.h"
#include "Game.h"
#include "util.h"

using namespace std;

static const char DELIMETER    = ',';
static const char SUBDELIMITER = '/';

unordered_map<string, GameObject> GameObject::gameObjects;


GameObject::GameObject(std::string name,
                       unsigned int mineralCost, unsigned int gasCost, unsigned int buildTime,
                       unsigned int supplyCost, unsigned int supplyProvided, unsigned int startEnergy,
                       unsigned int maxEnergy, unsigned int productionLines,
                       std::vector<std::string> producerNames, std::vector<std::string> dependencyNames,
                       BuildType buildType, bool isBuilding)
    : name(name), mineralCost(mineralCost), gasCost(gasCost), buildTime(buildTime),
      supplyCost(supplyCost), supplyProvided(supplyProvided),
      startEnergy(startEnergy), maxEnergy(maxEnergy), productionLines(productionLines),
      producerNames(producerNames), dependencyNames(dependencyNames), buildType(buildType),
      building(isBuilding) {}


GameObjectInstance& GameObject::addNewInstance(Game& game) {
    instances.emplace_back(*this, game.newInstanceID(), startEnergy, productionLines);
    return instances.back();
}


void GameObject::morphInstance(GameObjectInstance& source) {
    auto& sourceType = source.getType();
    auto& sourceInstances = sourceType.instances;
    auto it = find(sourceInstances.begin(), sourceInstances.end(), source);

    assert(it != sourceInstances.end());

    source.setType(*this);
    source.setDead(false);
    source.setEnergy(startEnergy);
    source.setFreeProductionLines(productionLines);

    instances.splice(instances.end(), sourceInstances, it);
}


/** @brief builds the techtree from a CSV file
 *
 * @throw TechTreeParsingException if the csv file could not be parsed
 *
 * @param filename the file to parse
 */
void GameObject::parseFile(string filename) {
    ifstream fs(filename);

    if (fs.is_open()) {
        parseStream(fs);
        fs.close();
    } else {
        throw TechTreeParsingException("Cannot open file");
    }
}

/** @brief builds the techtree from a string
 *
 * @throw TechTreeParsingException if the csv file could not be parsed
 *
 * @param filename the file to parse
 */
void GameObject::parseString(string input) {
    istringstream is(input);

    parseStream(is);
}


/** @brief builds the techtree from an input stream
 *
 * @throw TechTreeParsingException if the csv file could not be parsed
 *
 * @param filename the file to parse
 */
void GameObject::parseStream(istream &inputStream) {
    string line;
    unsigned int linecounter = 0;
    while (getline(inputStream, line)) {
        linecounter++;

        if(line.length() == 0 || line[0] == '#')
            continue;

        vector<string> tokens = split(line, DELIMETER, trim);

        if (tokens.size() < 13)
            throw TechTreeParsingException("Too few tokens", linecounter);

        vector<string> producers = split(tokens[9], SUBDELIMITER, trim);
        vector<string> dependencies = split(tokens[10], SUBDELIMITER, trim);

        if(producers[0] == "")
            producers.clear();
        if(dependencies[0] == "")
            dependencies.clear();
          
        gameObjects.insert(make_pair(tokens[0], GameObject(
            tokens[0],                   // name;
            stol(tokens[1]) * FP_FACTOR, // mineralCost
            stol(tokens[2]) * FP_FACTOR, // gasCost
            stol(tokens[3]),             // buildTime
            stod(tokens[4]) * FP_FACTOR, // supplyCost (uses stod because it can be 0.5)
            stod(tokens[5]) * FP_FACTOR, // supplyProvided (uses stod because it can be 0.5)

            stol(tokens[6]) * FP_FACTOR, // startEnergy
            stol(tokens[7]) * FP_FACTOR, // maxEnergy

            tokens[0].find("_with_reactor") != std::string::npos ? 2 : 1, //TODO: freeProductionLines

            //race, we don't need this (yet?)
            // tokens[8] == "terran" ? Race::TERRAN :
            //     (tokens[8] == "zerg" ? Race::ZERG :
            //     (tokens[8] == "protoss" ? Race::PROTOSS :
            //         throw TechTreeParsingException("Invalid race", linecounter))),

            producers,
            dependencies,

            tokens[11] == "morph" ? BuildType::MORPH :
                (tokens[11] == "pairmorph" ? BuildType::PAIRMORPH :
                    (tokens[11] == "active" ? BuildType::ACTIVE_BUILD :
                        (tokens[11] == "warp" ? BuildType::INSTANTIATE :
                            throw TechTreeParsingException("Invalid build type", linecounter)))),
            tokens[12][0] == '1' //isBuilding
        )));
    }
}


/** @brief gets a GameObject by its name
 *
 * @param name the name of the game object to get
 *
 * @throw out_of_range if no GameObject with this name exists
 *
 * @return a reference to the GameObject corresponding to this name
 */
GameObject& GameObject::get(const string name) {
    return GameObject::gameObjects.at(name);
}


vector<GameObjectInstance*> GameObject::getAll(function<bool(GameObjectInstance&)> filter) {
    vector<GameObjectInstance*> results;

    for(auto& go : gameObjects) {
        for(GameObjectInstance& goi : go.second.instances) {
            if(filter(goi))
                results.push_back(&goi);
        }
    }

    return results;
}


/** @brief gets a producer instance for this GameObject type.
 *  The producer is guaranteed to be not too busy (i.e. it can produce this object)
 *  and be a viable producer (i.e. it is allowed to produce this object)
 *
 *  @return a pointer to the possible producer or nullptr if there is no available producer
 */
GameObjectInstance* GameObject::getPossibleProducer() {
    static const auto canMorph = [](GameObjectInstance& inst) {
        // When mophing all production lines must be free
        return (!inst.isDead()) && (inst.getFreeProductionLines() == inst.getType().getProductionLines());
    };

    static const auto canActiveOrInstatiate = [](GameObjectInstance& inst) {
        return (!inst.isDead()) && (inst.getFreeProductionLines() > 0);
    };

    auto predicate = (buildType == BuildType::MORPH || buildType == BuildType::PAIRMORPH) ? canMorph : canActiveOrInstatiate;

    for(string producerName : producerNames) {
        GameObject& producerType = get(producerName);
        auto prod = find_if(producerType.begin(), producerType.end(), predicate);
        if (prod != producerType.end())
            return &(*prod);
    }
    return nullptr;
}


bool GameObject::areDependenciesMet() const {
    if (dependencyNames.size() == 0) return true;
    for(string dependencyName : dependencyNames) {
        if(get(dependencyName).instances.size() > 0) {
            return true;
        }
    }
    return false;
}


/** @brief gets the number of currently non busy instances of this GameObject
 *
 *  @return the number of the currently free instances
 */
unsigned int GameObject::getFreeInstancesCount() const {
    unsigned int free = 0;
    for (auto unit : instances)
        if ((!unit.isDead()) && (unit.getFreeProductionLines() > 0))
            ++free;
    return free;
}


GameObject::InstancesIter GameObject::begin() {
    return instances.begin();
}


GameObject::InstancesIter GameObject::end() {
    return instances.end();
}


void GameObject::increaseInstancesEnergy(int value) {
    for(auto& go : gameObjects){
        for(GameObjectInstance& goi : go.second.instances){
            goi.updateEnergy(value);
            if(goi.getEnergy() > go.second.maxEnergy)
                goi.setEnergy(go.second.maxEnergy);
        }
    }
}
