/**
 * @file main.cpp
 * @brief The main entrance of the simulation application
 * @details
 * @version 1.0
 * @date 3/13/2020
 * @details
 * Copyright 2014-2020 Huijie Qiao
 * The parameters to run the simulation are listed below.
 * 1. configure's base folder, which the application can load the configuration for species and scenario.
 * 2. scenario configuration. A JSON format configuration file to set up the parameter of the scenario
 * 3. species configuration. A JSON format configuration file to set up the parameter of the species in the simulation.
 * 4. result folder. A path to save the result
 * 5. memory limit (in MB). A number to set up the maximum memory allocate to the application
 * 6. is overwrite. An ZERO value means skip the simulation if the result folder exists and NONE-ZERO value means run the simulation no matter that the folder exists or not.
 * 7. with detail. An ZERO value means output the details of the simulation or not.
 *
 */

using namespace std;
#include <stdio.h>
#include <stdlib.h>
#include <execinfo.h>
#include <signal.h>
#include <unistd.h>
#include <algorithm>
#include <string>
#include <math.h>
#include <sqlite3.h>
#include <unistd.h>
#include <boost/thread/thread.hpp>

#include "Universal/easylogging.h"
#include "Universal/CommonFun.h"
#include "EES3D/Scenario.h"
#include "EES3D/ISEA3H.h"
#include "EES3D/Neighbor.h"
INITIALIZE_EASYLOGGINGPP;

int mainy(int argc, const char *argv[]) {
    int f = stoi(argv[1]);
    std::set<int> example = { 1};

    if (example.find(f) != example.end()) {
        std::cout << "Found\n";
    } else {
        std::cout << "Not found\n";
    }
    return 0;
}
/*-----------------------------------------
 * Main entrance for the simulation application
 * Parameters
 * 1. configure's base folder, which the application can load the configuration for species and scenario.
 * 2. scenario configuration. A JSON format configuration file to set up the parameter of the scenario
 * 3. species configuration. A JSON format configuration file to set up the parameter of the species in the simulation.
 * 4. result folder. A path to save the result
 * 5. memory limit (in MB). A number to set up the maximum memory allocate to the application
 * 6. is overwrite. An ZERO value means skip the simulation if the result folder exists and NONE-ZERO value means run the simulation no matter
 * that the folder exists or not.
 * 7. with detail. An ZERO value means output the details of the simulation or not.
 *
 *-----------------------------------------*/
int mainx(int argc, const char *argv[]) {
    //ISEA3H* t = new ISEA3H("/home/huijieqiao/git/ees__data/ISEA3H8/CSV/Debiased_Maximum_Monthly_Precipitation/0000.csv");
    //LOG(INFO) <<t->readByID(55);
    sqlite3 *env_db;
    string env_db_str = "/home/huijieqiao/git/ees__data/ISEA3H8/SQLITE/env_Hadley.sqlite";
    int rc = sqlite3_open(env_db_str.c_str(), &env_db);
    if (rc) {
        LOG(INFO) << "Can't open environment database: " << sqlite3_errmsg(env_db);
        exit(0);
    } else {
        LOG(INFO) << "Opened environment database from <" << env_db_str << "> successfully.";
    }

    Neighbor *neighborInfo = new Neighbor(env_db);
    /*
    set<int> neighbors;
    set<int> handled_ids;
    neighborInfo->getNeighborByID(240, 2, &neighbors, &handled_ids);
    for (unsigned id : neighbors) {
        LOG(INFO) << id;
    }
    for (auto it : neighborInfo->getNeighbors()){
        set<int> n ;
        set<int> h ;
        neighborInfo->getNeighborByID(it.first, 1, &n, &h);
        if (n.size()!=7){
            LOG(INFO)<<it.first<<" has "<<n.size()<<" neighbors.";
        }
        n.clear();
        //h.clear();

    }



    //CommonFun::freeContainer(neighbors);
    //CommonFun::freeContainer(handled_ids);
     * */
    size_t a = CommonFun::getCurrentRSS(1);
    LOG(INFO)<< a;
    delete neighborInfo;
    sqlite3_close(env_db);
    LOG(INFO)<< a - CommonFun::getCurrentRSS(1);
    //LOG(INFO) <<"distance is "<< neighborInfo->distance(22, 50145);
    //LOG(INFO) <<"distance is "<< neighborInfo->distance(31184, 31262, 10);


    return 0;
}
int main(int argc, const char *argv[]) {

    // Check the validity of the input
    // If the length of parameters are not satisfied with the required number, the application will skip this simulation and show out a warning.
    if (argc == 1) {
        printf("env_db, conf_db, base_folder, id(-1=all), memLimit(in GB), Overwrite\n");
        exit(1);
    }
    // Set up the timer.
    srand(static_cast<unsigned>(time(0)));
    string env_db = argv[1];
    string conf_db = argv[2];
    string target = argv[3];
    int id = atoi(argv[4]);
    int is_debug = atoi(argv[7]);
    //initialize the logger

    string LOGFILE = target + "/runtime.log";
    el::Configurations defaultConf;
    defaultConf.setToDefault();
    defaultConf.set(el::Level::Global, el::ConfigurationType::Enabled, "true");
    defaultConf.setGlobally(el::ConfigurationType::ToStandardOutput, "true");
    defaultConf.setGlobally(el::ConfigurationType::ToFile, "true");
    defaultConf.setGlobally(el::ConfigurationType::Filename, LOGFILE);
    el::Loggers::reconfigureLogger("default", defaultConf);
    el::Loggers::setDefaultConfigurations(defaultConf, true);

    el::Loggers::addFlag(el::LoggingFlag::HierarchicalLogging);
    if (is_debug == 1) {
        printf("Debug mode\n");
        el::Loggers::setLoggingLevel(el::Level::Global);
    } else {
        printf("Release mode\n");
        defaultConf.setGlobally(el::ConfigurationType::ToFile, "false");
        el::Loggers::reconfigureLogger("default", defaultConf);
        el::Loggers::setDefaultConfigurations(defaultConf, true);
        el::Loggers::setLoggingLevel(el::Level::Error);
    }
    el::Loggers::flushAll();

    unsigned long memory_limit = atoi(argv[5]);
    bool is_overwrite = atoi(argv[6]);
    //initialize the main scenario

    Scenario* a = new Scenario(env_db, conf_db, target, is_overwrite, id, memory_limit);
    delete a;


    return EXIT_SUCCESS;
}

