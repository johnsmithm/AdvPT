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
	GameObjectInstance &producingInstance;//need that larvae to be GameObjectInstance
public:
    
    BuildAction(GameObject &objectToBuild_)
        objectToBuild(objectToBuild_){}
    
	bool canExecute() {
        for(GameObject objectType : objectToBuild.possibleProducers)
            for(GameObjectInstance object : objectType.InstaceID)
                   if(object.notBussy() && Exist_resurses()){
                          producingInstance = object;
                          producingInstance.setBussy();
                            return true;
                     }
    };
	void start(){
          if(objectToBuild.buildType==INSTANTIATE){
              producingInstance.setFree();
              //Eliminate that worker for that second from harving minerals or gas
          }
          if(race==Zerg && objectToBuild.morphFromLarvae()){
          putNewLarvae();
          }
          CalculateFinishingTime();//checkMule
    };
    void finish(){
        switch(objectToBuild.buildType){
             case MORPH :  producingInstance.RemoveInstace();
             case ACTIVE_BUILD :  producingInstance.setFree();
        }        
    };
};

class BoostAction {//can a building be target again?
private:
	GameObjectInstance &target;
    void start(){
         //check Target Actions, change finishing time
        CalculateFinishingTime();
    }
    void finish(){
       target.unMarck();
    }
};

class CreateLarvaeHatcharyAction {//from eggs or from hatcharies??
private:
	GameObjectInstance &Hatchary;
    void finish(){
       ++NrLarvaue;
    }
}

class CreateLarvaeEggsAction {//from eggs or from hatcharies??
private:
	GameObjectInstance &Hatchary;
    void finish(){
       NrLarvaue+=4;
    }
}

class MuleAction {
private:
   GameObject& CommandCenter_;
   GameObject& OrbitalCommand_;
   GameObjectInstance &target;    
public:    
    
    MuleAction(GameObject& CommandCenter, GameObject& OrbitalCommand)
        CommandCenter_(CommandCenter),OrbitalCommand_(OrbitalCommand){}//can be inatializated at the begining
    
    bool canExecute(){//can be executed more time per second???
        for(GameObjectInstace Command : CommandCenter_.instanceID)
                 if(Command.hasEnergy()){
                    target = Command;
                    return true;
                 }
        //same for orbital
    }
    void start(){
     target.UpdateEnergy(-50*1000);    
     MuleIncrese = MineralRate * 4;  
     CalculateFinishingTime();    
    }
    void finish(){
     MuleIncrese = 0;
    }
};