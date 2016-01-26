#ifndef _SELECTOR_H_
#define _SELECTOR_H_

#include <functional>

using namespace std;
template<typename Gametype>
class Selector{
	public:
		Selector(string mode);
    void getBestBuildLists(vector<vector<string>>& newlists, vector<vector<string>>& bestLists);
	private:
		vector<pair<vector<string>, int>> lastBuildLists;

		string mode;
		std::function<int(int)> getCompareCriteria;
		int arraySize = 10;
};

void template<typename Gametype>
Selector::Selector(string setMode, string setTarget)
:mode(setMode), target(setTarget)
{
	if(mode == "rush"){
		getCompareCriteria = [](JsonValue output) {
			if(output["messages"].size() > 0){
      	return output["messages"][output["messages"].size() - 1]["time"];
			}
			return 1000;
		};
	}
	else {
		getCompareCriteria = [](JsonValue output) {
			if(output["messages"].size() > 0){
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
		g.readBuildList(list);
		g.simulate();
		JsonValue output = g.getOutput().getJson();
		if(output["messages"].size() > 0){
			int compareCriteria = getCompareCriteria(output);
			if(bestLists.size() > arraySize && bestLists[arraySize - 1].second > compareCriteria){
				bestLists.erase(bestLists.back());
			}
			size_t position = bestLists.size() - 1;
			while(position >= 0 && bestLists[position].second > compareCriteria)
				--position;
			//check the cases when it is last element and first one
			bestLists.insert(bestLists.begin() + position + 1, make_pair(list,compareCriteria));
		}
	}
}

#endif