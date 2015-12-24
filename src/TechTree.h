#ifndef _TECHTREE_H_
#define _TECHTREE_H_

#include <memory>
#include <vector>
#include "GameObject.h"

class TechTree {
public:

private:
	std::vector<std::shared_ptr<GameObject>> gameObjects;
};

#endif
