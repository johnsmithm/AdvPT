#ifndef _TECHTREE_H_
#define _TECHTREE_H_

#include <memory>
#include <vector>
#include <string>
#include "GameObject.h"
#include "BuildList.h"


enum class Race {
    TERRAN,
    PROTOS,
    ZERG
};


class TechTree {
public:
    /**
     * Creates an empty TechTree
     */
    TechTree() {}

    /**
     * Parse TechTree from csv-File
     */
    void parseFile(std::string filename);

    /**
     * Read BuildList from file using this TechTree
     */
    BuildList readBuildList(std::string filename);

private:
    std::vector<std::shared_ptr<GameObject>> gameObjects;
};

#endif
