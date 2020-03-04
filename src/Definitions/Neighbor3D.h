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
#include "../Universal/const.h"
#include "../Universal/CommonFun.h"
#include "../Universal/CSVReader.h"
/**
 *@brief A class to handle the Neighbor3D data
 */
class Neighbor3D {
private:
	boost::unordered_map<unsigned, set<int>> neighbors;
	string filename;
public:

	/**
	 * @brief Constructor of Neighbor3D class No.1
	 * @param p_filename the file to load the map info
	 */
	Neighbor3D(const string p_filename);

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
	void getNeighborByID(unsigned p_id, unsigned distance, set<int>* neighbors);

	unsigned distance(unsigned p_id1, unsigned p_id2, set<int>* neighbors);
	/**
	 * @brief return the file name of the 3D Neighbor3D object.
	 * @return
	 */
	string getFilename();

};

#endif /* Neighbor3D_H */
