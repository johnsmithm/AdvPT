#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "TechTree.h"


static const char DELIMETER    = ',';
static const char SUBDELIMITER = '/';


static std::vector<std::string> split(const std::string& str, char delimeter) {
    std::stringstream ss(str);
    std::string token;
    std::vector<std::string> tokens;
    while (std::getline(ss, token, delimeter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// static std::string trim(std::string s){

// }

void TechTree::parseFile(std::string filename) {
    std::string line;
    std::ifstream fs(filename);
    unsigned int linecounter = 0;
    if (fs.is_open()) {
        while (std::getline(fs, line) && (line.length() > 0) && (line[0] != '#')) {
            linecounter++;
            std::vector<std::string> tokens = split(line, DELIMETER);

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

                split(tokens[9], SUBDELIMITER), // producers
                split(tokens[10], SUBDELIMITER) // dependencies
                ))
            );
        }
        fs.close();
    } else {
        throw TechTreeParsingException("Cannot open file");
    }
}


/*BuildList TechTree::readBuildList(std::string filename) {

}*/
