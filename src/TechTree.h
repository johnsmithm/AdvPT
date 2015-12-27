#ifndef _TECHTREE_H_
#define _TECHTREE_H_

#include <exception>
#include <memory>
#include <string>
#include <vector>

#include "GameObject.h"
#include "BuildList.h"


enum class Race {
    TERRAN,
    PROTOSS,
    ZERG
};

class TechTreeParsingException : public std::runtime_error {
public:
    TechTreeParsingException(const std::string &msg)
        : std::runtime_error(msg){};

    TechTreeParsingException(const std::string &msg, int line)
        : std::runtime_error(msg + " on line " + std::to_string(line)){};
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
