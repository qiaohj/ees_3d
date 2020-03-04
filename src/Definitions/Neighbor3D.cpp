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

Neighbor3D::Neighbor3D(const string p_filename) {
    filename = p_filename;

    CSVReader reader(filename, " ", true);
    vector<vector<string> > data = reader.getData();
    for (vector<string> vec : data) {
        bool isFirst = true;
        unsigned key = 0;
        set<unsigned> values;
        for (string data : vec) {
            if (isFirst) {
                //LOG(INFO)<<"put "<<stoi(data)<<" to key";
                key = stoi(data);
                isFirst = false;
            } else {
                //LOG(INFO)<<"add "<<stoi(data)<<" to values";
                values.insert(stoi(data));
            }
        }
        neighbors[key] = values;

    }

}
void Neighbor3D::getNeighborByID(unsigned p_id, unsigned distance,
        set<unsigned> *p_neighbors, set<unsigned> *handled_ids) {
    //LOG(INFO)<<p_id<<" "<<distance;
    if (distance > 0) {
        set<unsigned> v = neighbors[p_id];
        handled_ids->insert(p_id);
        for (unsigned nei_id : v) {
            if (find(handled_ids->begin(), handled_ids->end(), nei_id) == handled_ids->end()) {
                getNeighborByID(nei_id, distance - 1, p_neighbors, handled_ids);
            }
        }
    }else{
        p_neighbors->insert(p_id);
    }
}
unsigned Neighbor3D::distance(unsigned p_id1, unsigned p_id2, unsigned limited) {
    unsigned distance = 0;
    while (true) {
        //LOG(INFO)<<"distance "<<distance;
        set<unsigned> neighbors;
        set<unsigned> handled_ids;
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
string Neighbor3D::getFilename() {
    return filename;
}

Neighbor3D::~Neighbor3D() {

}

