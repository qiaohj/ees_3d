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
    envName = p_env_name;
    boost::unordered_map<int, boost::unordered_map<int, float>> values =
            CommonFun::readEnvInfo(p_env_db, p_env_name, true);
    string sql = "SELECT * FROM environments WHERE names='" + p_env_name + "'";
    sqlite3_stmt *stmt;
    sqlite3_prepare(p_env_db, sql.c_str(), -1, &stmt, NULL);
    bool done = false;
    int begin_year = 0;
    int end_year = 0;
    int step = 0;
    while (!done) {
        switch (sqlite3_step(stmt)) {
        case SQLITE_ROW: {
            begin_year = sqlite3_column_int(stmt, ENVIRONMENTS_begin_year);
            end_year = sqlite3_column_int(stmt, ENVIRONMENTS_end_year);
            step = sqlite3_column_int(stmt, ENVIRONMENTS_step);
            break;
        }

        case SQLITE_DONE:
            done = true;
            break;

        default:
            done = true;
            LOG(INFO) << "SQLITE ERROR: " << sqlite3_errmsg(p_env_db);
        }
    }
    LOG(DEBUG) << "Begin year is " << begin_year << ". End year is " << end_year << ". Step is " << step << ".";
    int y1 = (begin_year>end_year)?end_year:begin_year;
    int y2 = (begin_year>end_year)?begin_year:end_year;
    LOG(DEBUG) << "do it from " << y1 << " to " << y2;
    for (int y = y1; y <=y2 ; y += step) {
        int key = y;
        ISEA3H *v = new ISEA3H(values[y]);
        LOG(DEBUG)<<"Initial environments information size is "<<values[y].size()<<" to key "<<key;
        layers[key] = v;
    }
    sqlite3_finalize(stmt);
}

ISEA3H* EnvironmentalISEA3H::getValues(int p_year) {
    LOG(DEBUG) << "Layer name is " << envName << ". Layer Size is " << layers.size();
    return layers[p_year];
}

float EnvironmentalISEA3H::readByID(int p_year, int p_id) {

    float value = layers[p_year]->readByID(p_id);
    return value;
}

EnvironmentalISEA3H::~EnvironmentalISEA3H() {

}

