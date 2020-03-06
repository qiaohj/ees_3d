/**
 * @file Neighbor3D.h
 * @brief Class Neighbor3D. A class to get the neighbors of a given face and distance
 * @author Huijie Qiao
 * @version 1.0
 * @date 3/3/2020
 * @details
 * Copyright 2014-2020 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 */

#ifndef Neighbor3D_H
#define Neighbor3D_H

using namespace std;
#include <string>
#include <iostream>
#include <fstream>
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/unordered_map.hpp>
#include <sqlite3.h>
#include "../Universal/const.h"
#include "../Universal/CommonFun.h"

/**
 *@brief A class to handle the Neighbor3D data
 */
class Neighbor3D {
private:
	boost::unordered_map<int, set<int>> neighbors;
public:

	/**
	 * @brief Constructor of Neighbor3D class No.1
	 * @param p_filename the file to load the map info
	 */
	Neighbor3D(sqlite3* env_db);

	/**
	 * @brief Destructor of Neighbor3D class
	 *
	 * release all the resources
	 */
	virtual ~Neighbor3D();

	/**
	 * @brief return all the neighbors of a given face within a given distance
	 * @return
	 */
	void getNeighborByID(int p_id, int distance, set<int>* neighbors, set<int> *handled_neighbors);

	int distance(int p_id1, int p_id2, int limited);

};

#endif /* Neighbor3D_H */
