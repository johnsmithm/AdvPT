#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include "Configuration.h"

static const char DELIMETER = ',';


std::vector<std::string> split(const std::string& str, char delimeter) {
    std::stringstream ss(str);
    std::string token;
    std::vector<std::string> tokens;
    while (std::getline(ss, token, delimeter)) {
        tokens.push_back(token);
    }
    return tokens;
}


void Configuration::parseGameObjects(std::string filename) {
    std::string line;
    std::ifstream fs(filename);
    if (fs.is_open()) {
        while (std::getline(fs, line) && (line.length() > 0) && (line[0] != '#')) {
            auto tokens = split(line, DELIMETER);

            if (tokens.size() < 3) throw "Invalid line!"; // TODO
            // ...
        }
        fs.close();
    } else {
        throw "Cannot open file"; // TODO
    }
}

void Configuration::parseBuildList(std::string filename,
                                   std::vector<BuildAction>& buildList) {

}
