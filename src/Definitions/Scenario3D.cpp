/**
 * @file Scenario3D.cpp
 * @brief Class Scenario3D. A class to define the features of a virtual scenario in a 3D simulation, and the virtual species in the scenario.
 * @author Huijie Qiao
 * @version 1.0
 * @date 3/3/2020
 * @details
 * Copyright 2014-2020 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 */

#include "Scenario3D.h"

Scenario3D::Scenario3D(string p_env_db, string p_conf_db, string p_target, bool p_overwrite, int p_id, unsigned long p_mem_limit) {


    //initialize the required parameters for the simulation.
    memLimit = p_mem_limit;

    //Load the environments layers for the simulation.
    LOG(INFO) << "Load environments";
    sqlite3* env_db = openDB(p_env_db);
    initEnvironments(env_db);

    LOG(INFO) << "Loading Neighbor list";
    neighborInfo = new Neighbor3D(env_db);

    sqlite3* conf_db = openDB(p_conf_db);

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
    vector<Simulation3D*> simulations = initSimulations(conf_db, env_db, p_id, p_target, p_overwrite, neighborInfo);

    LOG(INFO) << "Run the simulations. Simulation size is " << simulations.size();
    int i = 1;
    for (Simulation3D* simulation : simulations){
        bool status  = simulation->init(&environments_base, env_db, &masks);
        if (status){
            simulation->setIndexSimulation(i++);
            simulation->setTotalSimulation(simulations.size());
            simulation->run();
            simulation->commitLog();
            delete simulation;
        }
    }
    simulations.clear();
    sqlite3_close(env_db);
    sqlite3_close(conf_db);
    LOG(INFO) << "Finished";
}
sqlite3* Scenario3D::openDB(string p_db) {
    sqlite3 *env_db;
    int rc = sqlite3_open(p_db.c_str(), &env_db);
    if (rc) {
        LOG(INFO) << "Can't open database: " << sqlite3_errmsg(env_db) << " from " << p_db;
    } else {
        LOG(INFO) << "Opened database from <" << p_db;
    }
    return env_db;
}
void Scenario3D::initEnvironments(sqlite3* env_db) {
    string sql = "SELECT * FROM environments";
    sqlite3_stmt *stmt;

    sqlite3_prepare(env_db, sql.c_str(), -1, &stmt, NULL);
    bool done = false;
    while (!done) {
        int status = sqlite3_step(stmt);
        switch (status) {
        case SQLITE_ROW: {
            string env = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
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


vector<Simulation3D*> Scenario3D::initSimulations(sqlite3 *conf_db, sqlite3 *env_db, int p_id, string p_target, bool p_overwrite, Neighbor3D* neighborInfo) {
    vector<Simulation3D*> simulations;
    string sql;

    if (p_id == -1) {
        sql = "SELECT * FROM simulations WHERE is_run=1";
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
            string label = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, SIMULATION_label)));
            //LOG(DEBUG) << "init Species";
            SpeciesObject3D *new_species = new SpeciesObject3D(stmt, burn_in_year, timeLine);
            //LOG(DEBUG) << "Finished to init Species";

            string environments_str = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, SIMULATION_environments)));
            string mask_table = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, SIMULATION_mask)));
            vector<string> environment_labels = CommonFun::splitStr(environments_str, ",");

            Simulation3D *simulation = new Simulation3D(new_species, label, burn_in_year, p_target, p_overwrite, memLimit, timeLine, neighborInfo,
                    environment_labels, mask_table);

            /*-------------------
             * If the target folder exists and the is_overwrite parameter is false, skip the simulation,
             * or overwrite the existing result with the new simulation.
             -------------------------*/
            bool isFinished = boost::filesystem::exists(simulation->getTargetFolder());
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
    return simulations;
}

Scenario3D::~Scenario3D() {
    delete neighborInfo;

    for (auto it :environments_base){
        delete it.second;
    }

    for (auto it : masks){
        delete it.second;
    }

}


