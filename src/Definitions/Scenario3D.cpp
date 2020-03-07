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
    vector<Simulation3D*> simulations = initSimulations(conf_db, env_db, p_id, p_target, p_overwrite);

    for (Simulation3D* simulation : simulations){
        simulation->run();
    }


    sqlite3_close(env_db);
    sqlite3_close(conf_db);
    LOG(INFO) << "Finished";
}
sqlite3* Scenario3D::openDB(string p_db) {
    sqlite3 *env_db;
    int rc = sqlite3_open(p_db.c_str(), &env_db);
    if (rc) {
        LOG(INFO) << "Can't open database: " << sqlite3_errmsg(env_db) << " from " << p_db;
        exit(0);
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


vector<Simulation3D*> Scenario3D::initSimulations(sqlite3 *conf_db, sqlite3 *env_db, int p_id, string p_target, bool p_overwrite) {
    vector<Simulation3D*> simulations;
    string sql;
    boost::unordered_map<string, ISEA3H*> masks;
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
            Simulation3D *simulation = new Simulation3D();
            simulation->setOverwrite(p_overwrite);

            //simulation->setMemLimit(memLimit);
            int total_years = sqlite3_column_int(stmt, SIMULATION_total_years);
            simulation->setTotalYears(total_years);

            LOG(DEBUG) << "init Species";
            SpeciesObject3D *new_species = new SpeciesObject3D(stmt);
            simulation->addSpecies(new_species);
            LOG(DEBUG) << "Finished to init Species";
            simulation->addSpecies(new_species);

            string target = p_target + "/" + new_species->getLabel();
            simulation->setTargetFolder(target);

            bool isFinished = boost::filesystem::exists(target);

            /*-------------------
             * If the target folder exists and the is_overwrite parameter is false, skip the simulation,
             * or overwrite the existing result with the new simulation.
             -------------------------*/

            if ((isFinished) && (!p_overwrite)) {
                LOG(INFO) << "Result folder is exist, skip this simulation!";
                continue;
            }


            string environments_str = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, SIMULATION_environments)));

            vector<string> env_labels = CommonFun::splitStr(environments_str, ",");

            for (string env_label : env_labels) {
                simulation->addEnvironmentLabel(env_label);
                LOG(DEBUG) << "Trying to load environment " << env_label;
                EnvironmentalISEA3H *env = environments_base[env_label];
                if (env == NULL) {
                    LOG(DEBUG) << "No environment found, load it from db";
                    env = new EnvironmentalISEA3H(env_label, env_db);
                    environments_base[env_label] = env;
                }
                LOG(DEBUG) << "Finish to load the environment " << env_label;
                simulation->addEnvironment(env_label, env);
            }

            const string mask_table = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, SIMULATION_mask)));
            LOG(DEBUG) << "Loading mask";

            if (masks.find(mask_table) == masks.end()) {
                boost::unordered_map<int, boost::unordered_map<int, float>> mask_v = CommonFun::readEnvInfo(env_db, mask_table, false);
                ISEA3H *mask = new ISEA3H(mask_v[0]);
                masks[mask_table] = mask;
                simulation->setMask(mask);
            } else {
                simulation->setMask(masks[mask_table]);

            }
            LOG(DEBUG) << "Finished to load mask";

            LOG(DEBUG) << "Init simulation";
            simulation->init();
            //simulation->run();
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
    return simulations;
}

Scenario3D::~Scenario3D() {

}


