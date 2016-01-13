#include <fstream>
#include <sstream>
#include <iostream>

#include "GameObject.h"
#include "Game.h"
#include "util.h"

using namespace std;

unsigned int GameObjectInstance::maxID = 0;
unordered_map<string, shared_ptr<GameObject>> GameObject::gameObjects;

static const char DELIMETER    = ',';
static const char SUBDELIMITER = '/';



void GameObjectInstance::decreaseBusyness(){
    busyness--;
}

bool GameObjectInstance::isBusy() {
    return (busyness >= type.maxBusyness);
}

bool GameObjectInstance::hasEnergy(unsigned int val){
    return (val <= energy);
}

void GameObjectInstance::increaseBusyness(){
    busyness++;
}

void GameObjectInstance::setBusy(){
    busyness = type.maxBusyness;
}

void GameObjectInstance::updateEnergy(int val){ // val can be positive or negative
    energy += val;
}



std::ostream& operator<<(std::ostream &out, const GameObjectInstance &other){
    out << "GameObject " << other.type.getName() << " with ID " << other.ID;
    return out;
}


void GameObject::addNewInstance(Game &game){
    instances.push_back(GameObjectInstance(startEnergy, *this));

    game.setTotalSupplyAmount(game.getTotalSupplyAmount() + supplyProvided);
}

std::list<GameObjectInstance>& GameObject::getAllInstances(){
    return instances;
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
          
        gameObjects[tokens[0]] = make_shared<GameObject>(GameObject(
            tokens[0], // name;
            stol(tokens[1])*10000, // mineralCost
            stol(tokens[2])*10000, // gasCost
            stol(tokens[3]), // buildTime
            stol(tokens[4]), // supplyCost //seems to be an error for barracks_with_tech_lab
            stol(tokens[5]), // supplyProvided

            stol(tokens[6])*10000, // startEnergy
            stol(tokens[7])*10000, // maxEnergy

            tokens[0].find("_with_reactor") != std::string::npos? 2 : 1, //TODO: maxBusyness

            producers,
            dependencies,

            tokens[11] == "morph" ? BuildType::MORPH :
                (tokens[11] == "active" ? BuildType::ACTIVE_BUILD :
                (tokens[11] == "warp" ? BuildType::INSTANTIATE :
                    throw TechTreeParsingException("Invalid build type", linecounter))),
            tokens[12][0] == '1' //isBuilding
            ));
    }
}

/** @brief removes an instance from the game, freeing its supply
 */
void GameObject::removeInstance(GameObjectInstance instance, Game &game){
    game.setUsedSupplyAmount(game.getUsedSupplyAmount() - instance.type.supplyCost);
    game.setTotalSupplyAmount(game.getTotalSupplyAmount() - instance.type.supplyProvided);
    instances.remove(instance);   
}

/** @brief gets a GameObject by its name
 *
 * @param name the name of the game object to get
 *
 * @throw out_of_range if no GameObject with this name exists
 *
 * @return a reference to the GameObject corresponding to this name
 */
 GameObject& GameObject::get(const string name){
    return *GameObject::gameObjects.at(name);
 }


// bool catchAll

vector<GameObjectInstance*> GameObject::getAll(function<bool(GameObjectInstance&)> filter){
    vector<GameObjectInstance*> results;

    for(pair<string, shared_ptr<GameObject>> objectPointer : gameObjects){
        for(GameObjectInstance& goi : objectPointer.second->instances){
            if(filter(goi))
                results.push_back(&goi);
        }
    }

    return results;
}

/** @brief gets a possible producer instance for this GameObject type.
 *  The producer is guaranteed to be not too busy (i.e. it can produce this object)
 *  and be a viable producer (i.e. it is allowed to produce this object)
 *
 *  @return a pointer to the producer or nullptr if there is no available producer
 */
GameObjectInstance* GameObject::getPossibleProducer(){
    for(string producerName : producerNames){
        GameObject& producer = get(producerName);
        for(GameObjectInstance& goi : producer.instances){
            if(!goi.isBusy()){
              //cout<<goi.busyness<<" "<<goi.type.maxBusyness<<"\n";
                return &goi;
            }
        }
    }
    return nullptr;
}

bool GameObject::areDependenciesMet(){
    if (dependencyNames.size() == 0) return true;
    for(string dependencyName : dependencyNames){
        if(get(dependencyName).instances.size() > 0){
            return true;
        }
    }
    return false;
}

/** @brief gets the number of currently non busy instances of this GameObject
 *
 *  @return the number of the currently free instances
 */
unsigned int GameObject::getFreeInstancesCount() {
    unsigned int free = 0;
    for(auto unit : instances)
        if(!unit.isBusy())
            ++free;
    return free;
}

void GameObject::increaseEnergy(int amount){
    for(pair<string, shared_ptr<GameObject>> objectPointer : gameObjects){
        for(GameObjectInstance& goi : objectPointer.second->instances){
            goi.energy += amount;
            if(goi.energy > goi.type.maxEnergy)
                goi.energy = goi.type.maxEnergy;
        }
    }
}
