#ifndef _OUTPUT_H_
#define _OUTPUT_H_

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <string>

namespace pt = boost::property_tree;

class Output {
public:
	template<typename T> static void put(std::string key, T value){
		Output::tree.put(key, value);
	}

	template<typename T> static void addListItem(std::string key, T value, std::string subKey=""){
		pt::ptree emptyTree;
		pt::ptree &subTree = Output::tree.get_child(key, emptyTree);

		pt::ptree entry;
		entry.put(subKey, value);

		subTree.push_back(std::make_pair("", entry));
		Output::tree.put_child(key, subTree);

		//go to hell, boost
	}

	static void print(){
		pt::json_parser::write_json(std::cout, Output::tree);
	}

private:
	Output(){};
    Output(Output const&);
    void operator=(Output const&);

	// static Output singleton;
	static pt::ptree tree;

};

#endif