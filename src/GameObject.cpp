#include <fstream>

#include "GameObject.h"
#include "util.h"

unsigned int GameObjectInstance::maxID = 0;
std::vector<std::shared_ptr<GameObject>> GameObject::gameObjects;

static const char DELIMETER    = ',';
static const char SUBDELIMITER = '/';



bool GameObjectInstance::isBusy() {
    return (business == 0);
}

bool GameObjectInstance::hasEnergy(unsigned int val){
    return (val <= energy);
}

void GameObjectInstance::updateEnergy(int val){ // val can be positive or negative
    energy += val;
}

void GameObject::addNewInstance(){
    instances.push_back(GameObjectInstance(maxEnergy, *this));
}


/** @brief resolves the GameObjects of this Object's dependencies and producers by their name
 *
 *  Upon initialization of this Object not all dependencies and producer GameObjects might
 *  have been instanciated yet. Thus, only their names can be stored. After all GameObjects have
 *  been created, they can be resolved and stored using this function. This is mandatory before
 *  the object can be used! Resolving on startup instead of on use increases performance.
 *
 */
void GameObject::resolveNames(){
    producers.clear();
    for(auto pn : producerNames)
        producers.push_back(getGameObject(pn));

    dependencies.clear();
    for(auto dn : dependencyNames)
        dependencies.push_back(getGameObject(dn));
}


/** @brief builds the techtree from a CSV file
 *
 * @param filename the file to parse
 */
void GameObject::parseFile(std::string filename) {
    std::string line;
    std::ifstream fs(filename);
    unsigned int linecounter = 0;
    if (fs.is_open()) {
        while (std::getline(fs, line) && (line.length() > 0) && (line[0] != '#')) {
            linecounter++;
            std::vector<std::string> tokens = split(line, DELIMETER, trim);

            if (tokens.size() < 11)
                throw TechTreeParsingException("Too few tokens", linecounter);

            gameObjects.push_back(std::make_shared<GameObject>(GameObject(
                tokens[0], // name
                std::stol(tokens[1]), // mineralCost
                std::stol(tokens[2]), // gasCost
                std::stol(tokens[3]), // buildTime
                std::stol(tokens[4]), // supplyCost
                std::stol(tokens[5]), // supplyProvided

                std::stol(tokens[6]), // startEnergy
                std::stol(tokens[7]), // maxEnergy

                -1, //TODO: maxBusiness

                //race, we don't need this (yet?)
                // tokens[8] == "terran" ? Race::TERRAN :
                //     (tokens[8] == "zerg" ? Race::ZERG :
                //     (tokens[8] == "protoss" ? Race::PROTOSS :
                //         throw TechTreeParsingException("Invalid race", linecounter))),

                split(tokens[9], SUBDELIMITER, trim), // producers
                split(tokens[10], SUBDELIMITER, trim) // dependencies
                ))
            );
        }
        fs.close();
    } else {
        throw TechTreeParsingException("Cannot open file");
    }

    for(auto go : GameObject::gameObjects){
        try{
            (*go).resolveNames();
        }catch(std::out_of_range){
            throw TechTreeParsingException("Invalid reference");
        }
    }
}

/** @brief gets a GameObject by its name
 *
 * @param name the name of the game object to get
 *
 * @throw out_of_range if no GameObject with this name exists
 *
 * @return a reference to the object corresponding to this name
 */
 std::shared_ptr<GameObject> GameObject::getGameObject(const std::string name){
    for(std::shared_ptr<GameObject> &go : GameObject::gameObjects){
        if((*go).name == name){
            return go;
        }
    }

    throw std::out_of_range("no game object with this name");
 }
