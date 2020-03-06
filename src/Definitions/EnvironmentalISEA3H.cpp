/**
 * @file EnvironmentalISEA3H.cpp
 * @brief Class EnvironmentalISEA3H. A class to handle the environmental layers in ISEA3H format
 * @author Huijie Qiao
 * @version 1.0
 * @date 3/3/2020
 * @details
 * Copyright 2014-2020 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 */

#include "EnvironmentalISEA3H.h"
EnvironmentalISEA3H::EnvironmentalISEA3H(string p_env_name,
        sqlite3 *p_env_db) {
    boost::unordered_map<int, boost::unordered_map<int, float>> values =
            CommonFun::readEnvInfo(p_env_db, p_env_name, true);
    string sql = "SELECT * FROM environments WHERE names='" + p_env_name + "'";
    sqlite3_stmt *stmt;
    sqlite3_prepare(p_env_db, sql.c_str(), sizeof sql.c_str(), &stmt, NULL);
    bool done = false;
    int begin_year = 0;
    int end_year = 0;
    int step = 0;
    while (!done) {
        switch (sqlite3_step(stmt)) {
        case SQLITE_ROW: {
            begin_year = sqlite3_column_int(stmt, 0);
            end_year = sqlite3_column_int(stmt, 1);
            step = sqlite3_column_int(stmt, 3);
            break;
        }

        case SQLITE_DONE:
            done = true;
            break;

        default:
            LOG(INFO) << "SQLITE ERROR: " << sqlite3_errmsg(p_env_db);
        }
    }

    for (int y = begin_year; y >= end_year; y -= step) {
        int key = begin_year - y;
        ISEA3H *v = new ISEA3H(values[y]);
        //LOG(INFO)<<"Initial environments information size is "<<values[y].size()<<" to key "<<key;
        layers[key] = v;
        if (y == 0) {
            break;
        }
    }
    sqlite3_finalize(stmt);
}

ISEA3H* EnvironmentalISEA3H::getValues(int p_year) {
    //LOG(INFO)<<"get:"<<p_year;
    //LOG(INFO)<<" from "<<layers[p_year]->getFilename();

    return layers[p_year];
}

float EnvironmentalISEA3H::readByID(int p_year, int p_id) {

    float value = layers[p_year]->readByID(p_id);
    return value;
}

EnvironmentalISEA3H::~EnvironmentalISEA3H() {

}

