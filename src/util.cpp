#include <string>
#include <sstream>
#include <vector>

#include "util.h"

using namespace std;

vector<string> split(const string& str, char delimeter, string func (const string &)) {
    stringstream ss(str);
    string token;
    vector<string> tokens;
    while (getline(ss, token, delimeter)) {
        tokens.push_back(func!=NULL ? func(token) : token);
    }
    return tokens;
}

string trim(const string &input){
    string s(input);

    s.erase(0, s.find_first_not_of(" \t\n\r\v\f"));
    s.erase(s.find_last_not_of(" \t\n\r\v\f")+1);

    return s;
}