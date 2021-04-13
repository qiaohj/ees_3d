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
 * 7. is_debug 0/1
 * 8. with detail. 0/1 means output the details of the simulation or not.
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
#include "alglib/dataanalysis.h"
#include "alglib/stdafx.h"
using namespace alglib;

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
        vector<int> timeLine;
        LOG(INFO) << i << ". 2" << ": " << CommonFun::getCurrentRSS(1);
        for (int j = 1200; j <= 0; j++) {
            timeLine.push_back(j);
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
        timeLine.clear();
        LOG(INFO) << i << ". 6" << ": " << CommonFun::getCurrentRSS(1);
    }
    sqlite3_close(env_db);
    LOG(INFO)<<"END "<<": "<<CommonFun::getCurrentRSS(1);
}

void testMap(){
    unordered_map<int, int> a;
    a[1] = 2;
    LOG(INFO)<<a[1];
    a[1] = 3;
    LOG(INFO)<<a[1];
    a.insert({1,  4});
    LOG(INFO)<<a[1];
    a.erase(1);
    a.insert({1,  4});
    LOG(INFO)<<a[1];
}
void time_sei()
{
    //
    // Here we demonstrate SSA forecasting on some toy problem with clearly
    // visible linear trend and small amount of noise.
    //
    ssamodel s;
    //real_1d_array x = "[18.003,17.796,18.003,18.617,18.611,18.492,18.794,18.378,16.800,15.664]";
    real_1d_array x = "[18.492,18.794,18.378,16.800,15.664]";

    //
    // First, we create SSA model, set its properties and add dataset.
    //
    // We use window with width=3 and configure model to use direct SSA
    // algorithm - one which runs exact O(N*W^2) analysis - to extract
    // two top singular vectors. Well, it is toy problem :)
    //
    // NOTE: SSA model may store and analyze more than one sequence
    //       (say, different sequences may correspond to data collected
    //       from different devices)
    //
    ssacreate(s);
    ssasetwindow(s, 5);
    ssaaddsequence(s, x);
    ssasetalgotopkdirect(s, 2);

    //
    // Now we begin analysis. Internally SSA model stores everything it needs:
    // data, settings, solvers and so on. Right after first call to analysis-
    // related function it will analyze dataset, build basis and perform analysis.
    //
    // Subsequent calls to analysis functions will reuse previously computed
    // basis, unless you invalidate it by changing model settings (or dataset).
    //
    // In this example we show how to use ssaforecastlast() function, which
    // predicts changed in the last sequence of the dataset. If you want to
    // perform prediction for some other sequence, use ssaforecastsequence().
    //
    real_1d_array trend;
    ssaforecastlast(s, 3, trend);

    //
    // Well, we expected it to be [9,10,11]. There exists some difference,
    // which can be explained by the artificial noise in the dataset.
    //
    printf("%s\n", trend.tostring(2).c_str()); // EXPECTED: [9.0005,9.9322,10.8051]
}
int testMalloc_trim(int argc, const char *argv[]) {
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
    for (int j = 0; j < times; j++) {
        ISEA *a = new ISEA();
        int v1 = rand() % 10000000;
        for (int i = 0; i < v1; i++) {
            a->setValue(i, 1);
        }
        delete a;
        malloc_trim(0);
    }

    malloc_trim(0);
    sleep(100);
    return 0;
}

int testGenerateDistMatrix() {
    //ISEA* t = new ISEA("/home/huijieqiao/git/ees__data/ISEA3H8/CSV/Debiased_Maximum_Monthly_Precipitation/0000.csv");
    //LOG(INFO) <<t->readByID(55);
    sqlite3 *env_db;
    string env_db_str = "/home/huijieqiao/git/ees_3d_data/SMART_SPECIES/ISEA3H8/SQLITE/env_Hadley3D.sqlite";
    int rc = sqlite3_open(env_db_str.c_str(), &env_db);
    if (rc) {
        LOG(INFO) << "Can't open environment database: " << sqlite3_errmsg(env_db);
        exit(0);
    } else {
        LOG(INFO) << "Opened environment database from <" << env_db_str << "> successfully.";
    }

    Neighbor *neighborInfo = new Neighbor(env_db);

    vector<string> logs;

    for (int i = 1; i<=65612; i++){
        LOG(INFO)<<i;
        unordered_map<int, int> neighbors;
        neighborInfo->getNeighborByID(i, 4, neighbors);
        for (auto j : neighbors){
            if (i >= j.first) {
                continue;
            }
            char sql[30];
            sprintf(sql, "%d,%d,%d", i, j.first, j.second);
            string sql_c = sql;
            logs.push_back(sql_c);
        }
    }
    CommonFun::writeFile(logs, "/home/huijieqiao/git/ees_3d_data/SMART_SPECIES/ISEA3H8/SQLITE/neighbor_distancs.db");

    unordered_map<int, int> neighbors;
    int target = 1;
    int distance = 4;
    neighborInfo->getNeighborByID(target, distance, neighbors);
    for (auto it : neighbors) {
        LOG(INFO) << target<<" neighbor is "<<it.first <<" distance is "<<it.second;
    }


    sqlite3_close(env_db);
    //LOG(INFO) <<"distance is "<< neighborInfo->distance(1, 13202, 5);
    //LOG(INFO) <<"distance is "<< neighborInfo->distance(15992, 15912, 5);


    return 0;
}
int testNeighbor() {
    //ISEA* t = new ISEA("/home/huijieqiao/git/ees__data/ISEA3H8/CSV/Debiased_Maximum_Monthly_Precipitation/0000.csv");
    //LOG(INFO) <<t->readByID(55);
    sqlite3 *env_db;
    string env_db_str = "/home/huijieqiao/git/ees_3d_data/SMART_SPECIES/ISEA3H8/SQLITE/env_Hadley3D.sqlite";
    int rc = sqlite3_open(env_db_str.c_str(), &env_db);
    if (rc) {
        LOG(INFO) << "Can't open environment database: " << sqlite3_errmsg(env_db);
        exit(0);
    } else {
        LOG(INFO) << "Opened environment database from <" << env_db_str << "> successfully.";
    }

    Neighbor *neighborInfo = new Neighbor(env_db);

    vector<string> logs;
    unordered_map<int, int> neighbors4;
    neighborInfo->getNeighborByID(20032, 1, neighbors4);
    for (auto it : neighbors4) {
        LOG(INFO) << "4. " << it.first << " dist:" << it.second;
    }
    exit(1);
    for (int dist=0; dist<=4; dist++){
        clock_t start1, end1, start2, end2, start3, end3, start4, end4;
        double time_taken1 = 0;
        double time_taken2 = 0;
        double time_taken3 = 0;
        double time_taken4 = 0;
        for (int i=0; i<10000; i++){
            int j = static_cast<double>(rand()) * 60000 / static_cast<double>(RAND_MAX) + 1;

            start1 = clock();
            unordered_map<int, int> neighbors1;
            neighborInfo->getNeighborByID_loop(j, dist, neighbors1);
            end1 = clock();
            time_taken1 += double(end1 - start1);

            start2 = clock();
            unordered_map<int, int> neighbors2;
            set<int> handled;
            neighborInfo->getNeighborByID_recursive(j, dist , neighbors2, handled);
            end2 = clock();
            time_taken2 += double(end2 - start2);

            start3 = clock();
            unordered_map<int, int> neighbors3;
            neighborInfo->getNeighborByID_dict(j, dist, neighbors3);
            end3 = clock();
            time_taken3 += double(end3 - start3);

            start4 = clock();
            unordered_map<int, int> neighbors4;
            neighborInfo->getNeighborByID(j, dist, neighbors4);
            end4 = clock();
            time_taken4 += double(end4 - start4);

            if ((i==0)&&true){
                LOG(INFO)<<"1. "<<neighbors1.size()<<" 2."<<neighbors2.size()<<" 3."<<neighbors3.size()<<" 4."<<neighbors4.size();
            }
            if ((i==0)&&(dist<=1)){
                for (auto it : neighbors1) {
                    LOG(INFO) << "1. " << it.first <<" dist:"<<it.second;
                }

                for (auto it : neighbors2) {
                    LOG(INFO) << "2. " << it.first <<" dist:"<<it.second;
                }

                for (auto it : neighbors3) {
                    LOG(INFO) << "3. " << it.first << " dist:" << it.second;
                }

                for (auto it : neighbors4) {
                    LOG(INFO) << "4. " << it.first << " dist:" << it.second;
                }
            }
        }
        LOG(INFO)<<"DISTANCE:"<<dist<<" T1:"<<time_taken1 <<" T2:"<<time_taken2 << " T3:"<<time_taken3<<" T4:" << time_taken4;
    }

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
    bool is_detail = (atoi(argv[8])==1);
    //initialize the logger

    string LOGFILE = target + "/runtimexx.log";
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
    for (int i=0; i<1; i++){
        Scenario* a = new Scenario(env_db, conf_db, target, is_overwrite, id, memory_limit, is_detail);
        LOG(DEBUG)<<"MEMORY USAGE BEFORE RELEASE SCENARIO: "<<CommonFun::getCurrentRSS(1);
        delete a;
        LOG(DEBUG)<<"MEMORY USAGE AFTER RELEASE SCENARIO: "<<CommonFun::getCurrentRSS(1);
    }

    return EXIT_SUCCESS;
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
int main(int argc, const char *argv[]){
    //testEnvVar(stoi(argv[1]));

    //return 0;
    //time_sei();
    //exit(1);
    //testNeighbor();
    //testGenerateDistMatrix();
    int status = run(argc, argv);
    return status;

    //testMap();
    //return 0;
}


