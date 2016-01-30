#include <chrono>
#include <iostream>

//#define DIAGNOSE

#ifdef DIAGNOSE
	#define DIAGNOSE_NEXTGEN
	#define DIAGNOSE_GETBEST
	#define DIAGNOSE_SIMULATE
	#define DIAGNOSE_PRINT_RESULT
	#define DIAGNOSE_PRINT_STEPS
#endif


template<template <typename, typename> class Container>
void printBuildList(Container<std::string, std::allocator<std::string>> &input, ostream& stream=cerr){
    for(std::string line : input){
        stream << line << "->";
    }
    stream << input.size() << endl;
}

template<class R>
R measureTime(std::function<R()> toMeasure, std::string funcName=""){
	std::chrono::time_point<std::chrono::system_clock> start, end;
	start = std::chrono::high_resolution_clock::now();
	R r = toMeasure();
	end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed_seconds = end-start;

    if(funcName == "")
    	std::cout << "elapsed time: " << elapsed_seconds.count()*1000 << "ms\n";
    else
    	std::cout << funcName << " took: " << elapsed_seconds.count()*1000 << "ms\n";

    return r;
}

template<>
void measureTime(std::function<void()> toMeasure, std::string funcName){
	std::chrono::time_point<std::chrono::system_clock> start, end;
	start = std::chrono::high_resolution_clock::now();
	toMeasure();
	end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed_seconds = end-start;

    if(funcName == "")
    	std::cout << "elapsed time: " << elapsed_seconds.count()*1000 << "ms\n";
    else
    	std::cout << funcName << " took: " << elapsed_seconds.count()*1000 << "ms\n";
}
