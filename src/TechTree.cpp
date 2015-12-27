#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "TechTree.h"


static const char DELIMETER    = ',';
static const char SUBDELIMITER = '/';


/** @brief Splits a string at a specified delimiter
 *
 * @param str the string to split
 * @param delimiter the character to split at
 * @param func if specified, apply this function to the result tokens before returning them
 *
 * @return a vector of the result tokens between the delimiter
 */
static std::vector<std::string> split(const std::string& str, char delimeter, std::string func (const std::string &)=NULL) {
    std::stringstream ss(str);
    std::string token;
    std::vector<std::string> tokens;
    while (std::getline(ss, token, delimeter)) {
        tokens.push_back(func!=NULL ? func(token) : token);
    }
    return tokens;
}


/** @brief Removes whitespace from the beginning and end of a string
 *  Whitespace is: SPACE ( ), TAB (\t), NEWLINE (\n), CARRIAGE RETURN (\r),
 *  FORM FEED (\f) and VERTICAL TAB (\v)
 *
 * @param input the string to remove whitespace from
 *
 * @return the trimmed string
 */
static std::string trim(const std::string &input){
    std::string s(input);

    s.erase(0, s.find_first_not_of(" \t\n\r\v\f"));
    s.erase(s.find_last_not_of(" \t\n\r\v\f")+1);

    return s;
}

/** @brief builds the techtree from a CSV file
 *
 * @param filename the file to parse
 */
void TechTree::parseFile(std::string filename) {
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
}


/*BuildList TechTree::readBuildList(std::string filename) {

}*/
