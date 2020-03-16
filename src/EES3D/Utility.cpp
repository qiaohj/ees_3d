/**
 * @file Utility.cpp
 * @brief Class Utility. A class to implement the functions used in the multi-object.
 * @author Huijie Qiao
 * @version 1.0
 * @date 3/13/2020
 * @details
 * Copyright 2014-2020 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 **/

#include "Utility.h"

void Utility::readEnvInfo(sqlite3 *db, string tablename, bool with_year,
        unordered_map<int, ISEA*> *values) {
    string sql = "SELECT * FROM " + tablename;
    sqlite3_stmt *stmt;
   //LOG(INFO) << "Query: "<< sql;

    sqlite3_prepare(db, sql.c_str(), -1, &stmt, NULL);
    int year = 0;
    bool done = false;
    while (!done) {
        switch (sqlite3_step(stmt)) {
        case SQLITE_ROW:{
            if (with_year){
                year = sqlite3_column_int(stmt, ENVIRONMENT_year);
            }
            int id = sqlite3_column_int(stmt, ENVIRONMENT_global_id);
            float v = (float)sqlite3_column_double(stmt, ENVIRONMENT_v);
            if (values->find(year)==values->end()){
                values->insert({year, new ISEA()});
            }
            values->at(year)->setValue(id, v);

            break;
        }

        case SQLITE_DONE:
            done = true;
            break;

        default:
            LOG(INFO) << "SQLITE ERROR: " << sqlite3_errmsg(db) << ". SQL is " << sql;
        }
    }

    sqlite3_finalize(stmt);
}
