#include <vector>

#include "Action.h"

class Game{
private:
	int curTime = 0;

	int minerals = 0;
	int gas = 0;
	int supply = 0;
	int larva = 0;

	int exploitedGeysers = 0;
	int freeWorkers = 0;
	int mineralMiningWorkers = 0;
	int gasMiningWorkers = 0;

	std::vector<Action> runningActions;
	std::vector<Action> buildList;

	Race curRace;


public:
	enum Race {
		TERRAN,
		PROTOS,
		ZERG
	}
	void Race getRace();
	void timeStep();
};