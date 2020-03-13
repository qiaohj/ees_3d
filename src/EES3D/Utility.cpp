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

boost::unordered_map<int, boost::unordered_map<int, float>> Utility::readEnvInfo(sqlite3 *db, string tablename, bool with_year) {
    boost::unordered_map<int, boost::unordered_map<int, float>> values;
    string sql = "SELECT * FROM " + tablename;
    sqlite3_stmt *stmt;
   //LOG(INFO) << "Query: "<< q;

    sqlite3_prepare(db, sql.c_str(), -1, &stmt, NULL);
    int year = 0;
    bool done = false;
    while (!done) {
        switch (sqlite3_step(stmt)) {
        case SQLITE_ROW:{
            if (with_year){
                year = sqlite3_column_int(stmt, 2);
            }
            int id = sqlite3_column_int(stmt, 0);
            float v = (float)sqlite3_column_double(stmt, 1);
            //LOG(INFO)<<"year "<<year<<" id "<<id<<" v "<<v;
            values[year][id] = v;
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

    return values;
}
