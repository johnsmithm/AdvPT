#include <vector>

class GameObjectInstance {
private:
	static unsigned int maxID;

	unsigned int ID;
	unsigned int energy;
	unsigned int business;

	GameObject type;
};

class GameObject{
private:
	unsigned int mineralCost;
	unsigned int gasCost;
	unsigned int maxBusiness;
	unsigned int maxEnergy;

	GameObject possibleProducers[];
	GameObject dependencies[];

	BuildType buildType;

	std::vector<GameObjectInstance> instanceID;

public:
	enum BuildType {
		MORPH,
		ACTIVE_BUILD,
		INSTANTIATE
	}

	void addInstance();
	void removeInstance();
};