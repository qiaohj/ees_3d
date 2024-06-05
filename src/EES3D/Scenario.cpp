/**
 * @file Scenario.cpp
 * @brief Class Scenario. A class to define the features of a virtual scenario in a  simulation, and the virtual species in the scenario.
 * @author Huijie Qiao
 * @version 1.0
 * @date 3/3/2020
 * @details
 * Copyright 2014-2020 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 */

#include "Scenario.h"

Scenario::Scenario(string p_env_db, string p_conf_db, string p_target, bool p_overwrite, int p_id, unsigned long p_mem_limit, bool details) {
    //initialize the required parameters for the simulation.
    memLimit = p_mem_limit;

    //Load the environments layers for the simulation.
    LOG(INFO)<<"MEMORY USAGE BEFORE INIT ENVIRONMENTS: "<<CommonFun::getCurrentRSS(1);
    LOG(INFO) << "Load environments";
    sqlite3* env_db = openDB(p_env_db);
    initEnvironments(env_db);

    LOG(INFO)<<"MEMORY USAGE BEFORE INIT NEIGHBORS: "<<CommonFun::getCurrentRSS(1);
    LOG(INFO) << "Loading Neighbor list";
    neighborInfo = new Neighbor(env_db);

    sqlite3* conf_db = openDB(p_conf_db);

    LOG(INFO)<<"MEMORY USAGE BEFORE INIT TIMELINE: "<<CommonFun::getCurrentRSS(1);
    string sql = "SELECT * FROM timeline;";
    sqlite3_stmt *stmt;
    sqlite3_prepare(conf_db, sql.c_str(), -1, &stmt, NULL);
    bool done = false;
    int from = 0;
    int to = 0;
    int step = 0;
    while (!done) {
        switch (sqlite3_step(stmt)) {
        case SQLITE_ROW: {
            from = sqlite3_column_int(stmt, TIMELINE_from);
            to = sqlite3_column_int(stmt, TIMELINE_to);
            step = sqlite3_column_int(stmt, TIMELINE_step);
            break;
        }

        case SQLITE_DONE:
            done = true;
            break;

        default:
            done = true;
            LOG(INFO) << "SQLITE ERROR: " << sqlite3_errmsg(conf_db);
        }
    }
    sqlite3_finalize(stmt);

    if (from < to) {
        for (int i = from; i <= to; i += step) {
            this->timeLine.push_back(i);
        }
    } else {
        for (int i = from; i >= to; i += step) {
            this->timeLine.push_back(i);
        }
    }
    LOG(INFO) << "Init the simulations";
    LOG(INFO) << "MEMORY USAGE BEFORE INIT SMULATIONS: " << CommonFun::getCurrentRSS(1);
    vector<Simulation*> simulations;
    initSimulations(conf_db, env_db, p_id, p_target, p_overwrite, neighborInfo, simulations, details);

    LOG(INFO) << "Run the simulations. Simulation size is " << simulations.size();
    int i = 1;
    for (Simulation *simulation : simulations) {
        LOG(INFO) << "MEMORY USAGE BEFORE INIT SIMULATION (" << simulation->getTargetFolder() << "): " << CommonFun::getCurrentRSS(1);
        bool status = simulation->init(environments_base, env_db, masks);
        LOG(INFO) << "MEMORY USAGE AFTER INIT SIMULATION (" << simulation->getTargetFolder() << "): " << CommonFun::getCurrentRSS(1);
        if (status) {
            simulation->setIndexSimulation(i++);
            simulation->setTotalSimulation(simulations.size());
            LOG(INFO) << "MEMORY USAGE BEFORE RUN: " << CommonFun::getCurrentRSS(1);
            simulation->run();
            LOG(INFO) << "MEMORY USAGE BEFORE COMMIT LOG: " << CommonFun::getCurrentRSS(1);
            simulation->commitLog();

        }else{
            i++;
        }
        LOG(INFO) << "MEMORY USAGE BEFORE RELEASE: " << CommonFun::getCurrentRSS(1);
        delete simulation;
        LOG(INFO) << "MEMORY USAGE AFTER RELEASE: " << CommonFun::getCurrentRSS(1);
        malloc_trim(0);
        LOG(INFO) << "MEMORY USAGE AFTER RETURN MEMORY TO OS: " << CommonFun::getCurrentRSS(1);
        if ((CommonFun::getCurrentRSS(pow(1024, 3)) > memLimit)) {
            LOG(INFO) << "Current memory is " << CommonFun::getCurrentRSS(pow(1024, 3)) << "GB. Memory limit is " << memLimit << "GB";
            break;
        }
    }

    LOG(DEBUG) << "MEMORY USAGE AFTER ALL SIMULATIONS: " << CommonFun::getCurrentRSS(1);
    simulations.clear();
    sqlite3_close(env_db);
    sqlite3_close(conf_db);
    LOG(INFO) << "Finished";
    LOG(INFO)<<"MEMORY USAGE AT THE END "<<CommonFun::getCurrentRSS(1);
}
sqlite3* Scenario::openDB(string p_db) {
    sqlite3 *env_db;
    int rc = sqlite3_open(p_db.c_str(), &env_db);
    if (rc) {
        LOG(INFO) << "Can't open database: " << sqlite3_errmsg(env_db) << " from " << p_db;
    } else {
        LOG(INFO) << "Opened database from <" << p_db;
    }
    return env_db;
}
void Scenario::initEnvironments(sqlite3* env_db) {
    string sql = "SELECT * FROM environments";
    sqlite3_stmt *stmt;

    sqlite3_prepare(env_db, sql.c_str(), -1, &stmt, NULL);
    bool done = false;
    while (!done) {
        int status = sqlite3_step(stmt);
        switch (status) {
        case SQLITE_ROW: {
            string env = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, ENVIRONMENTS_names)));
            environments_base[env] = NULL;
            break;
        }

        case SQLITE_DONE:
            done = true;
            break;

        default:
            done = true;
            LOG(INFO) << "SQLITE ERROR: " << sqlite3_errmsg(env_db) << ". ERROR CODE IS " << status;
        }
    }

    sqlite3_finalize(stmt);
}

void Scenario::initSimulations(sqlite3 *conf_db, sqlite3 *env_db, int p_id, string p_target, bool p_overwrite,
        Neighbor* neighborInfo, vector<Simulation*> &simulations, bool details) {
    string sql;

    if (p_id == -1) {
        sql = "SELECT * FROM simulations WHERE is_run=1 and continent_id<=100 order by random()";
    } else {
        sql = "SELECT * FROM simulations WHERE is_run=1 and id=" + to_string(p_id);
    }
    sqlite3_stmt *stmt;
    LOG(DEBUG) << "Execute sql <" << sql << ">";
    sqlite3_prepare(conf_db, sql.c_str(), -1, &stmt, NULL);
    bool done = false;

    while (!done) {
        int status = sqlite3_step(stmt);
        switch (status) {
        case SQLITE_ROW: {
            int burn_in_year = sqlite3_column_int(stmt, SIMULATION_burn_in_year);
            int from = sqlite3_column_int(stmt, SIMULATION_from);
			int to = sqlite3_column_int(stmt, SIMULATION_to);
			int step = sqlite3_column_int(stmt, SIMULATION_step);
			int species_evo_type = sqlite3_column_int(stmt, SIMULATION_species_evo_type);
			double directional_speed = sqlite3_column_double(stmt, SIMULATION_directional_speed);
			int species_evo_level = sqlite3_column_int(stmt, SIMULATION_species_evo_level);

            string label = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, SIMULATION_label)));
            LOG(DEBUG) << "init Species";
            Species *new_species = new Species(stmt, burn_in_year, timeLine, from, to, step);
            LOG(DEBUG) << "Finished to init Species";

            string environments_str = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, SIMULATION_environments)));
            string mask_table = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, SIMULATION_mask)));
            int evo_type = sqlite3_column_int(stmt, SIMULATION_evo_type);


            vector<string> environment_labels = CommonFun::splitStr(environments_str, ",");
            LOG(DEBUG)<<"evo_type is "<<evo_type;
            Simulation *simulation = new Simulation(new_species, label, burn_in_year, p_target, p_overwrite, memLimit, timeLine, neighborInfo,
                    environment_labels, mask_table, details, evo_type, from, to, step, species_evo_type, directional_speed, species_evo_level);

            /*-------------------
             * If the target folder exists and the is_overwrite parameter is false, skip the simulation,
             * or overwrite the existing result with the new simulation.
             -------------------------*/
            bool isFinished = CommonFun::fileExist(simulation->getTargetFolder());
            if ((isFinished) && (!p_overwrite)) {
                LOG(INFO) << "Result folder is exist, skip this simulation!";
                delete simulation;
                continue;
            }

            simulations.push_back(simulation);
            break;
        }

        case SQLITE_DONE:
            done = true;
            break;

        default:
            done = true;
            LOG(ERROR) << "SQLITE ERROR: " << sqlite3_errmsg(env_db) << ". ERROR CODE IS " << status;
        }
    }

    sqlite3_finalize(stmt);
}

Scenario::~Scenario() {
    delete neighborInfo;

    for (auto it : environments_base){
        LOG(DEBUG)<<"DELETE ENVIRONMENT";
        delete it.second;
    }

    for (auto it : masks){
        LOG(DEBUG)<<"DELETE MASK";
        delete it.second;
    }
}


