#include <iostream>
#include <fstream>
#include <sstream>
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
    if (fs.is_open()) {
        while (std::getline(fs, line) && (line.length() > 0) && (line[0] != '#')) {
            auto tokens = split(line, DELIMETER);

            if (tokens.size() < 11) throw "Invalid line!"; // TODO
            // ...
        }
        fs.close();
    } else {
        throw "Cannot open file"; // TODO
    }
}


/*BuildList TechTree::readBuildList(std::string filename) {

}*/
