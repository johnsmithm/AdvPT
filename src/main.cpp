#include <iostream>
#include "Config.h"
#include "Game.h"
#include "Genetic.h"


#include <boost/program_options.hpp>
namespace po = boost::program_options;

using namespace std;

#include "defaultTechTree.tpl"

void printHelp(po::options_description visibleOpts){
        cout << "Usage: " << STR(BINARY_NAME) << " [options] <race> <build-list>" << endl;
        cout << "<race> can be any of the following: terran, zerg, protoss" << endl;
        cout << "<build-list> specifies the path to the build-list-file" << endl;
        cout << visibleOpts << endl;
}

void printVersion(){
    cout << STR(BINARY_NAME) << " v" << STR(VERSION_MAJOR) << "." << STR(VERSION_MINOR) << endl;
}

int main(int argc, char **argv){
    string buildListFilename;
    string techTreeFilename;
    string race;

    po::options_description visibleOpts("Options");
    visibleOpts.add_options()
        ("help,h", "show this help message and exit")
        ("version,v", "print the version and exit")
        ("tech-tree,t", po::value<string>(&techTreeFilename), "location of the tech tree config file")
    ;

    po::options_description hiddenOpts;
    hiddenOpts.add_options()
        ("race", po::value<string>(&race), "the race to simulate")
        ("build-list", po::value<string>(&buildListFilename), "location of the build list to simulate");
    ;

    po::positional_options_description positionalOpts;
    positionalOpts.add("race", 1);
    positionalOpts.add("build-list", 1);

    po::options_description allOpts;
    allOpts.add(visibleOpts).add(hiddenOpts);

    po::variables_map vm;
    try{
        po::store(
            po::command_line_parser(argc, argv)
                .options(allOpts)
                .positional(positionalOpts)
                .run(),
            vm);
        po::notify(vm);
    } catch ( const boost::program_options::error& e ) {
        cerr << e.what() << endl;
    }

    if (vm.count("help")) {
        printHelp(visibleOpts);
        return 0;
    }

    if(vm.count("version")){
        printVersion();
        return 0;
    }

    if (!vm.count("race") || !(race == "protoss" || race == "zerg" || race == "terran") ){
        cout << "Missing race or invalid race" << endl << endl;
        printHelp(visibleOpts);
        return 0;
    }

    if (!vm.count("build-list")){
        cout << "Missing build-list parameter" << endl << endl;
        printHelp(visibleOpts);
        return 0;
    }

    if (vm.count("tech-tree")){
        GameObject::parseFile(techTreeFilename);
    }else{
        GameObject::parseString(defaultTechTree);
    }
/*
    Game *g;

    if(race == "protoss"){
        g = new ProtossGame();
    }else if(race == "zerg"){
        g = new ZergGame();
    }else{
        g = new TerranGame();
    }

    try{
        g->readBuildList(buildListFilename);
    }catch(const SimulationException &e){
        cerr << e.what() << endl;
        exit(1);
    }
    
    g->simulate();
    g->getOutput().print();
*/
    Genetic s("protoss","push","colossus");
    s.findBuildList();
    s.printBest();
    //delete g;
}
