#ifndef _JSONOUTPUT_H_
#define _JSONOUTPUT_H_

#include "json/json.h"
#include "GameObject.h"

class BuildAction;
class BoostAction;
class MuleAction;
class QueenAction;
class Game;


class JsonOutput {
public:
    void gameInformation(Race race);

    void buildListPrecheck(bool isValid);

    void initSimulation();

    void emptyMessage(Game& game);

    void event(BuildAction& action);
    
    void event(BuildAction& action, unsigned int producedID);

    void event(BoostAction& action);

    void event(MuleAction& action);

    void event(QueenAction& action);

    void addGameGlobals(Game& game);

    void print(); // TODO replace with << operator

private:
    Json::Value json;

    Json::Value& basicEvent(Game& game);
};

#endif
