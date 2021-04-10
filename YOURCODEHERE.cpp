#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include <fstream>
#include <map>
#include <math.h>
#include <fcntl.h>
#include <vector>
#include <iterator>

#include "431project.h"

using namespace std;

/*
 * Enter your PSU IDs here to select the appropriate scanning order.
 */
#define PSU_ID_SUM (961890335 + 978478804)
//1940369139%24=3 so we choose order 3 ie BP core FPU cache 

/*
 * Some global variables to track heuristic progress.
 * 
 * Feel free to create more global variables to track progress of your
 * heuristic.
 */
unsigned int currentlyExploringDim = 0;
bool currentDimDone = false;
bool isDSEComplete = false;
int myOrder [] = {12,13,14,11,2,3,4,5,6,7,8,9,10,0,1};

/*
 * Given a half-baked configuration containing cache properties, generate
 * latency parameters in configuration string. You will need information about
 * how different cache paramters affect access latency.
 * 
 * Returns a string similar to "1 1 1"
 */
std::string generateCacheLatencyParams(string halfBackedConfig)
{
int dl1assoc = extractConfigPararm(halfBackedConfig, 4);
    int il1assoc = extractConfigPararm(halfBackedConfig, 6);
    int ul2assoc = extractConfigPararm(halfBackedConfig, 9);
    unsigned int dl1size = getdl1size(halfBackedConfig);
    unsigned int il1size = getil1size(halfBackedConfig);
    unsigned int ul2size = getl2size(halfBackedConfig);

    int il1latency;
    int dl1latency;
    int ul2latency;
    int dl1overhead;
    int il1overhead;
    int ul2overhead;

    switch (dl1size){
        case 2048:  dl1latency =  1;break;
        case 4096:  dl1latency =  2;break;
        case 8192:  dl1latency =  3;break;
        case 16384: dl1latency =  4;break;
        case 32768: dl1latency =  5;break;
        case 65536: dl1latency =  6;break;
        default:    dl1latency =  0;break;
    }
    switch (il1size){
        case 2048:  il1latency = 1;break;
        case 4096:  il1latency = 2;break;
        case 8192:  il1latency = 3;break;
        case 16384: il1latency = 4;break;
        case 32768: il1latency = 5;break;
        case 65536: il1latency = 6;break;
        default:    il1latency = 0;break;
    }
    switch (ul2size){
        case 32768:  ul2latency = 5;break;
        case 65536:  ul2latency = 6;break;
        case 131072: ul2latency = 7;break;
        case 262144: ul2latency = 8;break;
        case 524288: ul2latency = 9;break;
        case 1048576:ul2latency = 10;break;
        default:     ul2latency = 0;break;
    }
    switch (dl1assoc){
        case 0: dl1overhead=0;break;
        case 1: dl1overhead=1;break;
        case 2: dl1overhead=2;break;
        case 3: dl1overhead=3;break;
    }
    switch (il1assoc){
        case 0: il1overhead=0;break;
        case 1: il1overhead=1;break;
        case 2: il1overhead=2;break;
        case 3: il1overhead=3;break;
    }
    switch (ul2assoc){
        case 0: ul2overhead=0;break;
        case 1: ul2overhead=1;break;
        case 2: ul2overhead=2;break;
        case 3: ul2overhead=3;break;
        case 4: ul2overhead=4;break;
    }
    int dl1 = dl1latency + dl1overhead-1;
    int il1 = il1latency + il1overhead-1;
    int ul2 = ul2latency + ul2overhead-5;


    stringstream latencySettings;
    latencySettings<<dl1<<" "<<il1<<" "<<ul2;

	//
	//YOUR CODE BEGINS HERE
	//

	// This is a dumb implementation.
	//	latencySettings = "1 1 1";

	//
	//YOUR CODE ENDS HERE
	//

	return latencySettings.str();
	return latencySettings;
}

/*
 * Returns 1 if configuration is valid, else 0
 */
}

/*
 * Returns 1 if configuration is valid, else 0
 */
int validateConfiguration(std::string configuration)
{

	int l1blockSize;
    int width;
    int ul2blockSize;
    int fpwidthString = extractConfigPararm(configuration, 11);
    int l1blockString = extractConfigPararm(configuration, 2);
    int ul2blockString = extractConfigPararm(configuration, 8);
    switch (fpwidthString) {
        case 0: width=1;break;
        case 1: width=2;break;
        case 2: width=4;break;
        case 3: width=8;break;
    }
    switch (l1blockString){
        case 0: l1blockSize=8;break;
        case 1: l1blockSize=16;break;
        case 2: l1blockSize=32;break;
        case 3: l1blockSize=64;break;
    }
    switch (ul2blockString){
        case 0: ul2blockSize=16;break;
        case 1: ul2blockSize=32;break;
        case 2: ul2blockSize=64;break;
        case 3: ul2blockSize=128;break;
    }

    unsigned int dl1size = getdl1size(configuration);
    unsigned int il1size = getil1size(configuration);
    unsigned int ul2size = getl2size(configuration);
//    string latencies = generateCacheLatencyParams(configuration);



    if(l1blockSize >= width*8)
        return 0;
    if(2*l1blockSize>ul2blockSize)
        return 0;
    if((il1size<2048)||(dl1size<2048))
        return 0;
    if((il1size>65536)||(dl1size>65536))
        return 0;
    if((ul2size<32768)||(ul2size>1048576))
        return 0;
    if(!isNumDimConfiguration(configuration))
        return 0;
	// The below is a necessary, but insufficient condition for validating a
	// configuration.
	return 1;
	// The below is a necessary, but insufficient condition for validating a
	// configuration.
	return isNumDimConfiguration(configuration);
}

/*
 * Given the current best known configuration, the current configuration,
 * and the globally visible map of all previously investigated configurations,
 * suggest a previously unexplored design point. You will only be allowed to
 * investigate 1000 design points in a particular run, so choose wisely.
 *
 * In the current implementation, we start from the leftmost dimension and
 * explore all possible options for this dimension and then go to the next
 * dimension until the rightmost dimension.
 */
std::string generateNextConfigurationProposal(std::string currentconfiguration,
											  std::string bestEXECconfiguration, std::string bestEDPconfiguration,
											  int optimizeforEXEC, int optimizeforEDP)
{

	//
	// Some interesting variables in 431project.h include:
	//
	// 1. GLOB_dimensioncardinality
	// 2. GLOB_baseline
	// 3. NUM_DIMS
	// 4. NUM_DIMS_DEPENDENT
	// 5. GLOB_seen_configurations

	std::string nextconfiguration = currentconfiguration;
	// Continue if proposed configuration is invalid or has been seen/checked before.
	while (!validateConfiguration(nextconfiguration) ||
		   GLOB_seen_configurations[nextconfiguration])
	{

		// Check if DSE has been completed before and return current
		// configuration.
		if (isDSEComplete)
		{
			return currentconfiguration;
		}

		std::stringstream ss;

		string bestConfig;
		if (optimizeforEXEC == 1)
			bestConfig = bestEXECconfiguration;

		if (optimizeforEDP == 1)
			bestConfig = bestEDPconfiguration;

		// Fill in the dimensions already-scanned with the already-selected best
		// value.
		for (int dim = 0; dim < currentlyExploringDim; ++dim)
		{
			ss << extractConfigPararm(bestConfig, dim) << " ";
		} //TODO

		// Handling for currently exploring dimension. This is a very dumb
		// implementation.
		int nextValue = extractConfigPararm(nextconfiguration,
											currentlyExploringDim) +
						1;

		if (nextValue >= GLOB_dimensioncardinality[currentlyExploringDim])
		{
			nextValue = GLOB_dimensioncardinality[currentlyExploringDim] - 1;
			currentDimDone = true;
		} //TODO

		ss << nextValue << " ";

		// Fill in remaining independent params with 0.
		for (int dim = (currentlyExploringDim + 1);
			 dim < (NUM_DIMS - NUM_DIMS_DEPENDENT); ++dim)
		{
			ss << "0 ";
		} //fill with previous values instead of 0 //TODO

		//
		// Last NUM_DIMS_DEPENDENT3 configuration parameters are not independent.
		// They depend on one or more parameters already set. Determine the
		// remaining parameters based on already decided independent ones.
		//
		string configSoFar = ss.str();

		// Populate this object using corresponding parameters from config.
		ss << generateCacheLatencyParams(configSoFar);

		// Configuration is ready now.
		nextconfiguration = ss.str();

		// Make sure we start exploring next dimension in next iteration.
		if (currentDimDone)
		{
			currentlyExploringDim++;
			currentDimDone = false;
		} //make sure were navigating between dimensions correctly //TODO

		// Signal that DSE is complete after this configuration.
		if (currentlyExploringDim == (NUM_DIMS - NUM_DIMS_DEPENDENT))
			isDSEComplete = true; //TODO
	}
	return nextconfiguration;
}
