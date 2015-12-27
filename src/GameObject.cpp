#include "GameObject.h"

bool GameObjectInstance::isBusy() {
    return (business == 0);
}

bool GameObjectInstance::hasEnergy(unsigned int val){
    return (val <= energy);
}

void GameObjectInstance::updateEnergy(int val){ // val can be positive or negative
    energy += val;
}