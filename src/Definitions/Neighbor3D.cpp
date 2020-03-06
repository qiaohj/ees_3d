/**
 * @file Neighbor3D.cpp
 * @brief Class Neighbor3D. A class to handle the raster file
 * @author Huijie Qiao
 * @version 1.0
 * @date 11/25/2018
 * @details
 * Copyright 2014-2019 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 */

#include "Neighbor3D.h"
#include <iostream>
#include <fstream>
#include "../Universal/const.h"
#include "../Universal/log.hpp"

Neighbor3D::Neighbor3D(sqlite3* env_db) {
    string sql = "SELECT * FROM neighbor";
    sqlite3_stmt *stmt;

    sqlite3_prepare(env_db, sql.c_str(), sizeof sql.c_str(), &stmt, NULL);
    bool done = false;
    while (!done) {
        switch (sqlite3_step(stmt)) {
        case SQLITE_ROW: {
            int key = (int) sqlite3_column_int(stmt, 0);
            string neighbor =
                    string(
                            reinterpret_cast<const char*>(sqlite3_column_text(
                                    stmt, 1)));
            vector<string> tokens = CommonFun::splitStr(neighbor, ",");
            set<int> values;
            for (string token : tokens){
                values.insert(stoul(token));
            }
            neighbors[key] = values;
            break;
        }

        case SQLITE_DONE:
            done = true;
            break;

        default:
            LOG(INFO) << "SQLITE ERROR: " << sqlite3_errmsg(env_db);
        }
    }

    sqlite3_finalize(stmt);
}
void Neighbor3D::getNeighborByID(int p_id, int distance,
        set<int> *p_neighbors, set<int> *handled_ids) {
    //LOG(INFO)<<p_id<<" "<<distance;
    if (distance > 0) {
        set<int> v = neighbors[p_id];
        handled_ids->insert(p_id);
        for (int nei_id : v) {
            if (find(handled_ids->begin(), handled_ids->end(), nei_id) == handled_ids->end()) {
                getNeighborByID(nei_id, distance - 1, p_neighbors, handled_ids);
            }
        }
    }

    p_neighbors->insert(p_id);

}
int Neighbor3D::distance(int p_id1, int p_id2, int limited) {
    int distance = 0;
    while (true) {
        //LOG(INFO)<<"distance "<<distance;
        set<int> neighbors;
        set<int> handled_ids;
        getNeighborByID(p_id1, distance, &neighbors, &handled_ids);
        if (find(neighbors.begin(), neighbors.end(), p_id2) == neighbors.end()) {
            getNeighborByID(p_id1, ++distance, &neighbors, &handled_ids);
            if (distance>=limited){
                return distance;
            }
        }else{
            return distance;
        }
    }
}

Neighbor3D::~Neighbor3D() {

}

