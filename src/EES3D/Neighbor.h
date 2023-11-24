/**
 * @file Neighbor.h
 * @brief Class Neighbor. A class to get the neighbors of a given face and distance
 * @author Huijie Qiao
 * @version 1.0
 * @date 3/3/2020
 * @details
 * Copyright 2014-2020 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 */

#ifndef Neighbor_H
#define Neighbor_H

using namespace std;
#include <string>
#include <iostream>
#include <fstream>
#include <sqlite3.h>
#include <set>
#include "../Universal/Const.h"
#include "../Universal/CommonFun.h"
#include "DBField.h"

/**
 *@brief A class to handle the Neighbor data
 */
class Neighbor {
private:
    //unordered_map<int, set<int>> neighbors;
    unordered_map<int, unordered_map<int, set<int>>> distances;
public:

    /**
     * @brief Constructor of Neighbor class No.1
     * @param p_filename the file to load the map info
     */
    Neighbor(sqlite3 *env_db);

    /**
     * @brief Destructor of Neighbor class
     *
     * release all the resources
     */
    virtual ~Neighbor();

    /**
     * @brief return all the neighbors of a given face within a given distance
     * @return
     */
    void getNeighborByID(int p_id, int distance, unordered_map<int, int> &neighbors);
    void getNeighborByID_dict(int p_id, int distance, unordered_map<int, int> &neighbors);
    void getNeighborByID_loop(int p_id, int distance, unordered_map<int, int> &neighbors);
    void getNeighborByID_recursive(int p_id, int distance, unordered_map<int, int> &p_neighbors, set<int> &handled_ids);
    //int distance(int p_id1, int p_id2, int limited);
    unordered_map<int, set<int>> getNeighbors();
};

#endif /* Neighbor_H */
