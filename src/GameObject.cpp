#include "GameObject.h"

unsigned int GameObjectInstance::maxID = 0;

bool GameObjectInstance::isBusy() {
    return (business == 0);
}

bool GameObjectInstance::hasEnergy(unsigned int val){
    return (val <= energy);
}

void GameObjectInstance::updateEnergy(int val){ // val can be positive or negative
    energy += val;
}

void GameObject::addNewInstance(){
    instances.push_back(GameObjectInstance(maxEnergy, *this));
}


/** @brief resolves the GameObjects of this Object's dependencies and producers by their name
 *
 *  Upon initialization of this Object not all dependencies and producer GameObjects might
 *  have been instanciated yet. Thus, only their names can be stored. After all GameObjects have
 *  been created, they can be resolved and stored using this function. This is mandatory before
 *  the object can be used! Resolving on startup instead of on use increases performance.
 *
 *  @param tt the TechTree this Object and its dependencies are stored in
 */
void GameObject::resolveNames(TechTree tt){
    producers.clear();
    for(auto pn : producerNames)
        producers.push_back(tt.getGameObject(pn));

    dependencies.clear();
    for(auto dn : dependencyNames)
        dependencies.push_back(tt.getGameObject(dn));
}