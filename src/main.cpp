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
#include <malloc.h>

#include "Universal/easylogging.h"
#include "Universal/CommonFun.h"
#include "EES3D/Scenario.h"
#include "EES3D/ISEA.h"
#include "EES3D/Neighbor.h"
#include "EES3D/EnvVar.h"

INITIALIZE_EASYLOGGINGPP;




int testFind(int argc, const char *argv[]) {
    int f = stoi(argv[1]);
    set<int> example = { 1};

    if (example.find(f) != example.end()) {
        cout << "Found\n";
    } else {
        cout << "Not found\n";
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
int testSQLITE(int argc, const char *argv[]) {
    //ISEA* t = new ISEA("/home/huijieqiao/git/ees__data/ISEA3H8/CSV/Debiased_Maximum_Monthly_Precipitation/0000.csv");
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


int testISEA(int argc, const char *argv[]){
    clock_t start, end;
    start = clock();
    LOG(INFO)<<"Pointer";
    for (int j = 0; j < 3; j++) {
        map<int, int> *a = new map<int, int>();
        LOG(INFO) << 1 << ". " << CommonFun::getCurrentRSS(1);
        for (int i=0;i <100000; i++){
            a->erase(i);
            a->insert({i, 1});
        }
        LOG(INFO) << 2 << ". " << CommonFun::getCurrentRSS(1);
        a->clear();
        delete a;
        LOG(INFO) << 4 << ". " << CommonFun::getCurrentRSS(1);
    }
    class C {
    private:
        map<int, float> *v;
    public:
        C(){
            v = new map<int, float>();
        }
        virtual ~C(){
            //CommonFun::freeContainerRemoved(v);
            delete v;
        }
        void setValue(int p_id, float p_value){
            v->erase(p_id);
            v->insert({p_id, p_value});
        }
    };
    LOG(INFO) << "Object";
    for (int j = 0; j < 3; j++) {
        ISEA *a = new ISEA();
        LOG(INFO) << 1 << ". " << CommonFun::getCurrentRSS(1);
        for (int i = 0; i < 100000; i++) {
            a->setValue(i, 1);
        }
        LOG(INFO) << 2 << ". " << CommonFun::getCurrentRSS(1);

        delete a;
        LOG(INFO) << 4 << ". " << CommonFun::getCurrentRSS(1);
    }
    end = clock();
    double time_taken = double(end - start) / double(CLOCKS_PER_SEC);
    LOG(INFO)<<5<<". Time Taken:"<<time_taken;
    return 0;
}
int run(int argc, const char *argv[]) {

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
    LOG(DEBUG)<<"MEMORY USAGE BEFORE INIT SCENARIO: "<<CommonFun::getCurrentRSS(1);
    Scenario* a = new Scenario(env_db, conf_db, target, is_overwrite, id, memory_limit);
    LOG(DEBUG)<<"MEMORY USAGE BEFORE RELEASE SCENARIO: "<<CommonFun::getCurrentRSS(1);
    //delete a;
    LOG(DEBUG)<<"MEMORY USAGE AFTER RELEASE SCENARIO: "<<CommonFun::getCurrentRSS(1);


    return EXIT_SUCCESS;
}

void testEnvVar(int times){
    LOG(INFO)<<"BEGIN"<<": "<<CommonFun::getCurrentRSS(1);
    string p_db = "/home/huijieqiao/git/ees_3d_data/ISEA3H8/SQLITE/env_Hadley3D.sqlite";
    string p_env_name1 = "Debiased_Mean_Annual_Precipitation";
    string p_env_name2 = "Debiased_Mean_Annual_Temperature";
    sqlite3 *env_db;
    int rc = sqlite3_open(p_db.c_str(), &env_db);
    if (rc) {
        LOG(INFO) << "Can't open database: " << sqlite3_errmsg(env_db) << " from " << p_db;
    } else {
        LOG(INFO) << "Opened database from <" << p_db;
    }
    for (int i = 1; i <= times; i++) {
        LOG(INFO) << i << ". 1" << ": " << CommonFun::getCurrentRSS(1);
        vector<int> *timeLine = new vector<int>();
        LOG(INFO) << i << ". 2" << ": " << CommonFun::getCurrentRSS(1);
        for (int j = 1200; j <= 0; j++) {
            timeLine->push_back(j);
        }
        LOG(INFO) << i << ". 3.1" << ": " << CommonFun::getCurrentRSS(1);
        EnvVar *env1 = new EnvVar(p_env_name1, env_db);
        LOG(INFO) << i << ". 4.1" << ": " << CommonFun::getCurrentRSS(1);
        delete env1;
        LOG(INFO) << i << ". 5.1" << ": " << CommonFun::getCurrentRSS(1);

        LOG(INFO) << i << ". 3.2" << ": " << CommonFun::getCurrentRSS(1);
        EnvVar *env2 = new EnvVar(p_env_name2, env_db);
        LOG(INFO) << i << ". 4.2" << ": " << CommonFun::getCurrentRSS(1);
        delete env2;
        LOG(INFO) << i << ". 5.2" << ": " << CommonFun::getCurrentRSS(1);

        delete timeLine;
        LOG(INFO) << i << ". 6" << ": " << CommonFun::getCurrentRSS(1);
    }
    sqlite3_close(env_db);
    LOG(INFO)<<"END "<<": "<<CommonFun::getCurrentRSS(1);
}

int main(int argc, const char *argv[]){
    //testEnvVar(stoi(argv[1]));

    //return 0;
    int status = run(argc, argv);
    return status;
}

int mainxxx(int argc, const char *argv[]) {
	class C {
	private:
		map<int, float> v;
	public:
		C() {
			//v = new map<int, float>();
		}
		virtual ~C() {
			//CommonFun::freeContainerRemoved(v);
			//delete v;
			malloc_trim(0);
		}
		void setValue(int p_id, float p_value) {
			v[p_id] = p_value;
		}
	};
	int times = stoi(argv[1]);
	LOG(INFO) << "Object";
	for (int j = 0; j < times; j++) {
		ISEA *a = new ISEA();
		int v1 = rand() % 10000000;
		LOG(INFO) <<j<<". "<< 1 << ". " << CommonFun::getCurrentRSS(1) <<" SIZE is "<<v1;
		for (int i = 0; i < v1; i++) {
			a->setValue(i, 1);
		}
		LOG(INFO) <<j<<". "<< 2 << ". " << CommonFun::getCurrentRSS(1) <<" SIZE is "<<v1;

		delete a;
		LOG(INFO) <<j<<". "<< 4 << ". " << CommonFun::getCurrentRSS(1) <<" SIZE is "<<v1;
		malloc_trim(0);
		LOG(INFO) <<j<<". "<< 5 << ". " << CommonFun::getCurrentRSS(1) <<" SIZE is "<<v1;
	}

	malloc_trim(0);
	LOG(INFO)<< "6" << CommonFun::getCurrentRSS(1);
	sleep(100);
}

