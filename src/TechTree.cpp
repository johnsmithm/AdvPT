#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "TechTree.h"


static const char DELIMETER = ',';


static std::vector<std::string> split(const std::string& str, char delimeter) {
    std::stringstream ss(str);
    std::string token;
    std::vector<std::string> tokens;
    while (std::getline(ss, token, delimeter)) {
        tokens.push_back(token);
    }
    return tokens;
}



void TechTree::parseFile(std::string filename) {
    std::string line;
    std::ifstream fs(filename);
    unsigned int linecounter = 0;
    if (fs.is_open()) {
        while (std::getline(fs, line) && (line.length() > 0) && (line[0] != '#')) {
            linecounter++;
            auto tokens = split(line, DELIMETER);

            if (tokens.size() < 11)
                throw TechTreeParsingException("Too few tokens", linecounter);
            // ...
        }
        fs.close();
    } else {
        throw TechTreeParsingException("Cannot open file");
    }
}


/*BuildList TechTree::readBuildList(std::string filename) {

}*/
