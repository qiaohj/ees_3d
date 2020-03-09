/**
 * @file main.cpp
 * @brief The main entrance of the simulation application
 * @details
 * Copyright 2014-2019 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
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
#include <gdal.h>
#include <gdal_priv.h>
#include <ogr_srs_api.h>
#include <ogr_spatialref.h>
#include <stdio.h>
#include <stdlib.h>
#include <execinfo.h>
#include <signal.h>
#include <unistd.h>

//#include "JsonPaster/include/json/json.h"
#include <algorithm> // sort
#include <string>
#include <math.h>
#include <sqlite3.h>
#include <unistd.h>
#include <boost/thread/thread.hpp>

#include "ExpressionParser/parser.h"
#include "Definitions/Scenario3D.h"
#include "Universal/easylogging.h"
#include "Universal/CommonFun.h"
#include "Definitions/IndividualOrganism.h"
#include "Definitions/ISEA3H.h"
#include "Definitions/Neighbor3D.h"
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
    //ISEA3H* t = new ISEA3H("/home/huijieqiao/git/ees_3d_data/ISEA3H8/CSV/Debiased_Maximum_Monthly_Precipitation/0000.csv");
    //LOG(INFO) <<t->readByID(55);
    sqlite3 *env_db;
    string env_db_str = "/home/huijieqiao/git/ees_3d_data/ISEA3H8/SQLITE/env_Hadley3D.sqlite";
    int rc = sqlite3_open(env_db_str.c_str(), &env_db);
    if (rc) {
        LOG(INFO) << "Can't open environment database: " << sqlite3_errmsg(env_db);
        exit(0);
    } else {
        LOG(INFO) << "Opened environment database from <" << env_db_str << "> successfully.";
    }
    Neighbor3D *neighborInfo = new Neighbor3D(env_db);
    //exit(1);
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
    //LOG(INFO) <<"distance is "<< neighborInfo->distance(22, 50145);
    //LOG(INFO) <<"distance is "<< neighborInfo->distance(31184, 31262, 10);
    return 0;
}
int main(int argc, const char *argv[]) {

    // Check the validity of the input
    // If the length of parameters are not satisfied with the required number, the application will skip this simulation and show out a warning.
    if (argc == 1) {
        printf("env_db, conf_db, base_folder, id(-1=all), memory_limit(in M), is_overwrite\n");
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

    Scenario3D* a = new Scenario3D(env_db, conf_db, target, is_overwrite, id, memory_limit);
    //a->run();


    return EXIT_SUCCESS;
}

