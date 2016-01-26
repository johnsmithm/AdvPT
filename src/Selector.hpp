#ifndef _SELECTOR_H_
#define _SELECTOR_H_

#include <functional>

using namespace std;
template<typename Gametype>
class Selector{
	public:
		Selector(string mode, string setTarget);
    void getBestBuildLists(vector<vector<string>>& newlists, vector<vector<string>>& bestLists);
	private:
		//vector<pair<vector<string>, int>> lastBuildLists;

		string mode;
		string target;
		std::function<int(int)> getCompareCriteria;
		int arraySize = 10;
};

void template<typename Gametype>
Selector::Selector(string setMode, string setTarget)
:mode(setMode), target(setTarget)
{
	if(mode == "rush"){
		getCompareCriteria = [](JsonValue output) {
			if(output["buildlistValid"] == "1" && output["messages"].size() > 0){
      	return output["messages"][output["messages"].size() - 1]["time"];
			}
			return 1000;
		};
	}
	else {
		getCompareCriteria = [](JsonValue output) {
			if(output["buildlistValid"] == "1" && output["messages"].size() > 0){
				int count = 0;
      	for(auto message : output["messages"])
      		for(auto event : message["events"])
      			if(event["type"] == "build-start" && event["name"] == target)
      				++count;
      	return -count;
			}
			return 1000;
		};
	}
}

void template<typename Gametype>
Selector::getBestBuildLists(vector<vector<string>>& newlists, vector<pair<vector<string>, int>>& bestLists){
	//lastBuildLists = bestLists;
	for(auto list : newlists){
		Gametype g;
		g.readBuildList(list);//TODO
		g.simulate();
		JsonValue output = g.getOutput().getJson();
		if(output["buildlistValid"] == "1" && output["messages"].size() > 0){
			int compareCriteria = getCompareCriteria(output);
			if(bestLists.size() > arraySize ){
				if(bestLists[arraySize - 1].second <= compareCriteria)continue;
				bestLists.erase(bestLists.back());
			}
			size_t position = bestLists.size();
			// Find the position where buildList fits(Could use binary search).
			while(position > 0 && bestLists[--position].second > compareCriteria);

			bestLists.insert(bestLists.begin() + position, make_pair(list,compareCriteria));
		}
	}
}

#endif