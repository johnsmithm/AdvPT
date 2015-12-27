#ifndef _BUILDLIST_H_
#define _BUILDLIST_H_

#include <memory>
#include <vector>
#include <string>
#include "GameObject.h"

class BuildList {
public:
    /**
     * Creates an empty BuildList
     */
    BuildList() {}

private:
    std::vector<std::shared_ptr<GameObject>> gameObjects;
};

#endif
