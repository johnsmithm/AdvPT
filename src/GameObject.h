#include <vector>

class GameObjectInstance {
private:
	static unsigned int maxID;//--???

	unsigned int ID;
	unsigned int energy;
	unsigned int business;
    bool MuleTarget;//??

	GameObject type;
public:
    bool hasEnergy(unsigned int val){
      return val <= energy;
    }
    void UpdateEnergy(int val){ // val can be positive or negative
      energy += val;
    }    
};

class GameObject{
private:
	unsigned int mineralCost;
	unsigned int gasCost;
        unsigned int buildTime;//
        unsigned int suplyCost;//
        unsigned int larvaCost;//?? bad aproach
	unsigned int maxBusiness;
	unsigned int maxEnergy;
    
    std::string name;//how fast to find element by name
    
    unsigned int blockedInstaces;//will block Instances from left to right

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
    
    void create(){
    //read line from file
    }

	void addInstance();
	void removeInstance();
};
