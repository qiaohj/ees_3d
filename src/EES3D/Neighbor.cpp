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
    sqlite3_stmt *stmt1;
    string sql = "SELECT * FROM neighbor";
    sqlite3_prepare(env_db, sql.c_str(), -1, &stmt1, NULL);
    bool done = false;
    while (!done) {
        int status = sqlite3_step(stmt1);
        //LOG(INFO)<<"SQLITE3 status is :"<<status;
        switch (status) {
        case SQLITE_ROW: {
            int key = (int) sqlite3_column_int(stmt1, NEIGHBOR_ID);
            string neighbor = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt1, NEIGHBOR_NEIGHBOR)));
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

    sqlite3_finalize(stmt1);

    sqlite3_stmt *stmt2;
    sql = "SELECT * FROM distances";
    sqlite3_prepare(env_db, sql.c_str(), -1, &stmt2, NULL);
    done = false;
    while (!done) {
        int status = sqlite3_step(stmt2);
        //LOG(INFO)<<"SQLITE3 status is :"<<status;
        switch (status) {
        case SQLITE_ROW: {
            int i = (int) sqlite3_column_int(stmt2, DISTANCES_I);
            int j = (int) sqlite3_column_int(stmt2, DISTANCES_J);
            int dist = (int) sqlite3_column_int(stmt2, DISTANCES_DIST);
            distances[i][dist].insert(j);
            distances[j][dist].insert(i);
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

    sqlite3_finalize(stmt2);
}
void Neighbor::getNeighborByID(int p_id, int distance, unordered_map<int, int> &p_neighbors) {
    getNeighborByID_dict(p_id, distance, p_neighbors);
    /*
    if (distance>1){
        getNeighborByID_dict(p_id, distance, p_neighbors);
    }else{
        set<int> handled_ids;
        getNeighborByID_recursive(p_id, distance, p_neighbors, handled_ids);
    }*/
}
void Neighbor::getNeighborByID_dict(int p_id, int distance, unordered_map<int, int> &p_neighbors) {
    for (int i = 0; i <= distance; i++) {
        if (i==0){
            p_neighbors[p_id] = i;
        }else{
            for (auto it : distances[p_id][i]){
                p_neighbors[it] = i;
            }
        }
    }
}
void Neighbor::getNeighborByID_loop(int p_id, int distance, unordered_map<int, int> &p_neighbors) {
    set<int> v;
    set<int> handled_ids;
    v.insert(p_id);
    handled_ids.insert(p_id);

    for (int i = 0; i <= distance; i++) {
        //neighbors of cells in v;
        set<int> new_nei;
        for (int id : v){
            set<int> n = neighbors[id];
            for (int n_id : n){
                if (handled_ids.find(n_id)==handled_ids.end()){
                    new_nei.insert(n_id);
                    handled_ids.insert(n_id);
                }
            }
            p_neighbors[id] = i;
        }
        v = new_nei;
    }
}

//note: distance must <=1; or it will lost some neighbors. But it is the fastest way when distance <=1 and the result is correct.
void Neighbor::getNeighborByID_recursive(int p_id, int distance, unordered_map<int, int> &p_neighbors, set<int> &handled_ids) {
    //LOG(DEBUG)<<"ID is "<<p_id<<" and distance is "<<distance;
    if (distance > 0) {
        set<int> v = neighbors[p_id];
        handled_ids.insert(p_id);
        for (int nei_id : v) {
            if (handled_ids.find(nei_id) == handled_ids.end()) {
                getNeighborByID_recursive(nei_id, distance - 1, p_neighbors, handled_ids);
            }
        }
    }
    //LOG(DEBUG)<<"add "<<p_id<<" to list ";
    p_neighbors[p_id] = distance;

}
unordered_map<int, set<int>> Neighbor::getNeighbors(){
    return this->neighbors;
}
/*
int Neighbor::distance(int p_id1, int p_id2, int limited) {
    int distance = 0;
    while (true) {
        //LOG(INFO)<<"distance "<<distance;
        set<int> neighbors;
        set<int> handled_ids;
        getNeighborByID(p_id1, distance, neighbors, handled_ids);
        if (find(neighbors.begin(), neighbors.end(), p_id2) == neighbors.end()) {
            if (find(neighbors.begin(), neighbors.end(), p_id2) == neighbors.end()) {
                getNeighborByID(p_id1, ++distance, neighbors, handled_ids);
                if (distance > limited) {
                    return -1;
                }
            } else {
                return distance;
            }
        } else {
            return distance;
        }
    }
}
*/
Neighbor::~Neighbor() {
    neighbors.clear();
}

