#include <fstream>
#include <sstream>
#include <iostream>

#include "GameObject.h"
#include "util.h"

using namespace std;

unsigned int GameObjectInstance::maxID = 0;
unordered_map<string, shared_ptr<GameObject>> GameObject::gameObjects;

static const char DELIMETER    = ',';
static const char SUBDELIMITER = '/';



bool GameObjectInstance::isBusy() {
    return business < type.maxBusiness;
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

        if (tokens.size() < 11)
            throw TechTreeParsingException("Too few tokens", linecounter);

        gameObjects[tokens[0]] = (make_shared<GameObject>(GameObject(
            tokens[0], // name
            stol(tokens[1]), // mineralCost
            stol(tokens[2]), // gasCost
            stol(tokens[3]), // buildTime
            stol(tokens[4]), // supplyCost
            stol(tokens[5]), // supplyProvided

            stol(tokens[6]), // startEnergy
            stol(tokens[7]), // maxEnergy

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
}

/** @brief gets a GameObject by its name
 *
 * @param name the name of the game object to get
 *
 * @throw out_of_range if no GameObject with this name exists
 *
 * @return a reference to the shared pointer to the GameObject corresponding to this name
 */
 shared_ptr<GameObject>& GameObject::getGameObject(const string name){
    return GameObject::gameObjects.at(name);
 }

/** @brief gets a possible producer instance for this GameObject type.
 *  The producer is guaranteed to be not too busy (i.e. it can produce this object)
 *  and be a viable producer (i.e. it is allowed to produce this object)
 *
 *  @param result a reference to an object to write the possible producer to
 *
 *  @return true if a producer was found, false if there is no available producer
 */
bool GameObject::getPossibleProducer(GameObjectInstance* result){
    for(string producerName : producerNames){
        auto producer = getGameObject(producerName);
        for(GameObjectInstance goi : producer->instances){
            if(!goi.isBusy()){
                result = &goi;
                return true;
            }
        }
    }
    result = NULL;
    return false;
}

bool GameObject::areDependenciesMet(){
    for(string dependencyName : dependencyNames){
        if(getGameObject(dependencyName)->instances.size() == 0){
            return false;
        }
    }
    return true;
}