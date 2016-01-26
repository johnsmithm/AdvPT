#ifndef _SELECTOR_H_
#define _SELECTOR_H_

using namespace std;
template<typename Gametype>
class Selector{
	public:
		Selector(string mode);
    void getBestBuildLists(vector<vector<string>>& lists);
	private:
		vector<vector<string>> lastBuildLists;

		string mode;
};

#endif