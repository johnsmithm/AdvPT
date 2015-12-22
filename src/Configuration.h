#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

#include <string>
#include <vector>
#include <unordered_map>
#include "GameObject.h"
#include "Action.h"

class Configuration {
public:
    Configuration() {}

    void parseGameObjects(std::string filename);

    void parseBuildList(std::string filename, std::vector<BuildAction>& buildList);

private:
    std::unordered_map<std::string, GameObject> protossMap;
    std::unordered_map<std::string, GameObject> zergMap;
    std::unordered_map<std::string, GameObject> terranMap;
};


#endif
