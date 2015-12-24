#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "GameObject.h"
#include "TechTree.h"
#include "Action.h"

class Configuration {
public:
    TechTree parseGameObjects(std::string filename);

    std::vector<std::shared_ptr<BuildAction>>
    parseBuildList(std::string filename, TechTree& techTree);

private:
};


#endif
