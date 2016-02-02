#include <list>
#include <iostream>
#include "Config.h"
#include "Game.h"
#include "Creator.hpp"
#include "Selector.hpp"


#include <boost/program_options.hpp>
namespace po = boost::program_options;

using namespace std;

#include "defaultTechTree.tpl"

int main_optimization(int argc, char **argv);
int main_simulation(int argc, char **argv);

void printVersion(){
    cout << STR(BINARY_NAME) << " v" << STR(VERSION_MAJOR) << "." << STR(VERSION_MINOR) << endl;
}

int main(int argc, char **argv){

    if(argc >= 2 && string(argv[1]) == "simulate"){
        argv[1] = argv[0];
        argv++;
        argc--;
        exit(main_simulation(argc, argv));
    }else if(argc >= 2 && string(argv[1]) == "optimize"){
        argv[1] = argv[0];
        argv++;
        argc--;
        exit(main_optimization(argc, argv));
    }else{
        cout << "Usage: " << STR(BINARY_NAME) << " (simulate|optimize) [options] <mode-specific-parameters>" << endl;
        cout << "to display <mode-specific-parameters> use " << endl;
        cout << STR(BINARY_NAME) << " (simulate|optimize) --help" << endl;
    }


}

void printHelp_optimization(po::options_description visibleOpts){
        cout << "Usage: " << STR(BINARY_NAME) << " optimize [options] <race> <mode> <target>" << endl;
        cout << "<race> can be any of the following: terran, zerg, protoss" << endl;
        cout << "<mode> the mode to optimize (either 'push' or 'rush')" << endl;
        cout << "<target> the target to optimize (e.g. 'colossus')" << endl;
        cout << visibleOpts << endl;
}

int main_optimization(int argc, char **argv){
    string techTreeFilename;
    string race;
    string mode_string;
    string target;

    po::options_description visibleOpts("Options");
    visibleOpts.add_options()
        ("help,h", "show this help message and exit")
        ("version,v", "print the version and exit")
        ("tech-tree,t", po::value<string>(&techTreeFilename), "location of the tech tree config file")
    ;

    po::options_description hiddenOpts;
    hiddenOpts.add_options()
        ("race", po::value<string>(&race), "the race to optimize")
        ("mode", po::value<string>(&mode_string), "the mode to optimize (push|rush)")
        ("target", po::value<string>(&target), "the target to optimize");
    ;

    po::positional_options_description positionalOpts;
    positionalOpts.add("race", 1);
    positionalOpts.add("mode", 1);
    positionalOpts.add("target", 1);

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
        printHelp_optimization(visibleOpts);
        return 0;
    }

    if(vm.count("version")){
        printVersion();
        return 0;
    }

    if (!vm.count("race") || !(race == "protoss" || race == "zerg" || race == "terran") ){
        cout << "Missing race or invalid race" << endl << endl;
        printHelp_optimization(visibleOpts);
        return 1;
    }

    if (!vm.count("mode") || !(mode_string == "push" || mode_string == "rush")){
        cout << "Missing mode or invalid mode" << endl << endl;
        printHelp_optimization(visibleOpts);
        return 1;
    }

    if(!vm.count("target")){
        cout << "Missing target" << endl << endl;
        printHelp_optimization(visibleOpts);
        return 1;
    }

    if (vm.count("tech-tree")){
        GameObject::parseFile(techTreeFilename);
    }else{
        GameObject::parseString(defaultTechTree);
    }

    OptimizationMode mode = mode_string == "push" ? OptimizationMode::PUSH : OptimizationMode::RUSH;

    try{
        switch(raceFromString(race)){
            case Race::PROTOSS: {
                int itarations = 7000;
                if(target == "colossus")itarations = 200;
                if(target == "void_ray")itarations = 400;
                Selector<ProtossGame> selector(mode, target);
                selector.optimize(itarations);
                return 0;
            }
            case Race::ZERG: {
                Selector<ZergGame> selector(mode, target);
                selector.optimize(7000);
                return 0;
            }
            case Race::TERRAN:{
                Selector<TerranGame> selector(mode, target);
                selector.optimize(7000);
                return 0;
            }
        }
    }catch(const SimulationException &e){
        cerr << e.what() << endl;
        return 1;
    }

    return 1;
}

void printHelp_simulation(po::options_description visibleOpts){
        cout << "Usage: " << STR(BINARY_NAME) << " simulate [options] <race> <build-list>" << endl;
        cout << "<race> can be any of the following: terran, zerg, protoss" << endl;
        cout << "<build-list> specifies the path to the build-list-file" << endl;
        cout << visibleOpts << endl;
}

int main_simulation(int argc, char **argv){
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
        printHelp_simulation(visibleOpts);
        return 0;
    }

    if(vm.count("version")){
        printVersion();
        return 0;
    }

    if (!vm.count("race") || !(race == "protoss" || race == "zerg" || race == "terran") ){
        cout << "Missing race or invalid race" << endl << endl;
        printHelp_simulation(visibleOpts);
        return 1;
    }

    if (!vm.count("build-list")){
        cout << "Missing build-list parameter" << endl << endl;
        printHelp_simulation(visibleOpts);
        return 1;
    }

    if (vm.count("tech-tree")){
        GameObject::parseFile(techTreeFilename);
    }else{
        GameObject::parseString(defaultTechTree);
    }

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

        g->simulate();
        g->getOutput().print();

    }catch(const SimulationException &e){
        cerr << e.what() << endl;
        return 1;
    }

    delete g;

    return 0;
}