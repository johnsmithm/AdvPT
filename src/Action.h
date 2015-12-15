class Action {
protected:
	unsigned int finishTime;

public:
	virtual bool canExecute() = 0;
	virtual void start() = 0;
	virtual void finish() = 0;
};

class BuildAction {
private:
	GameObject &objectToBuild;
	GameObjectInstance &producingInstance;
};

class BoostAction {
private:
	GameObjectInstance &target;
};

class CreateLarvaeAction {
private:
	GameObjectInstance &target;
}