/**
 * @file Neighbor.cpp
 * @brief Class Neighbor. A class to handle the raster file
 * @author Huijie Qiao
 * @version 1.0
 * @date 3/13/2020
 * @details
 * Copyright 2014-2020 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 */

#include "Neighbor.h"


Neighbor::Neighbor(sqlite3 *env_db) {
    sqlite3_stmt *stmt;
    string sql = "SELECT * FROM neighbor";
    sqlite3_prepare(env_db, sql.c_str(), -1, &stmt, NULL);
    bool done = false;
    while (!done) {
        int status = sqlite3_step(stmt);
        //LOG(INFO)<<"SQLITE3 status is :"<<status;
        switch (status) {
        case SQLITE_ROW: {
            int key = (int) sqlite3_column_int(stmt, NEIGHBOR_ID);
            string neighbor = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, NEIGHBOR_NEIGHBOR)));
            vector<string> tokens = CommonFun::splitStr(neighbor, ",");
            set<int> values;
            for (string token : tokens) {
                values.insert(stoul(token));
            }
            neighbors[key] = values;
            break;
        }

        case SQLITE_DONE:
            done = true;
            break;

        default:
            done = true;
            LOG(INFO) << "SQLITE ERROR: " << sqlite3_errmsg(env_db);
        }
    }

    sqlite3_finalize(stmt);
}
void Neighbor::getNeighborByID(int p_id, int distance, set<int> *p_neighbors, set<int> *handled_ids) {
    //LOG(DEBUG)<<"ID is "<<p_id<<" and distance is "<<distance;
    if (distance > 0) {
        set<int> v = neighbors[p_id];
        handled_ids->insert(p_id);
        for (int nei_id : v) {
            if (handled_ids->find(nei_id) == handled_ids->end()) {
                getNeighborByID(nei_id, distance - 1, p_neighbors, handled_ids);
            }
        }
    }

    p_neighbors->insert(p_id);

}
boost::unordered_map<int, set<int>> Neighbor::getNeighbors(){
    return this->neighbors;
}
int Neighbor::distance(int p_id1, int p_id2, int limited) {
    int distance = 0;
    while (true) {
        //LOG(INFO)<<"distance "<<distance;
        set<int> neighbors;
        set<int> handled_ids;
        getNeighborByID(p_id1, distance, &neighbors, &handled_ids);
        if (find(neighbors.begin(), neighbors.end(), p_id2) == neighbors.end()) {
            getNeighborByID(p_id1, ++distance, &neighbors, &handled_ids);
            if (distance >= limited) {
                return distance;
            }
        } else {
            return distance;
        }
    }
}

Neighbor::~Neighbor() {

    for (auto it : neighbors){
        CommonFun::freeContainerRemoved(it.second);
    }
    CommonFun::freeContainerRemoved(neighbors);

}
