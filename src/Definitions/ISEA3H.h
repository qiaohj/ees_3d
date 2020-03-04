/**
 * @file ISEA3H.h
 * @brief Class ISEA3H. A class to handle the ISEA3H format file
 * @author Huijie Qiao
 * @version 1.0
 * @date 3/3/2020
 * @details
 * Copyright 2014-2020 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 */

#ifndef ISEA3H_H
#define ISEA3H_H

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
 *@brief A class to handle the ISEA3H data
 */
class ISEA3H {
private:
	boost::unordered_map<unsigned, float> values;
	string filename;
	unsigned cellSize;
public:
	/**
	 * @brief Constructor of ISEA3H class No.2 (Create a blank map)
	 */
	ISEA3H();
	/**
	 * @brief Constructor of ISEA3H class No.1 (load an ISEA3H from a file)
	 * @param p_filename the file to load the map info
	 */
	ISEA3H(const string p_filename);

	/**
	 * @brief Destructor of ISEA3H class
	 *
	 * release all the resources
	 */
	virtual ~ISEA3H();

	/**
	 * @brief read value by ID of face
	 * @return
	 */
	float readByID(unsigned p_id);

	/**
	 * @brief return the number of faces.
	 * @return
	 */
	unsigned getCellSize();

	/**
	 * @brief return all the IDs of the face.
	 * @return
	 */
	unsigned* getIDs();

	/**
	 * @brief set value to a given face by id.
	 * @return
	 */
	void setValue(unsigned p_id, float p_value);
	/**
	 * @brief return the face/value map.
	 * @return
	 */
	boost::unordered_map<unsigned, float> getValues();

	/**
	 * @brief return the file name of the 3D ISEA3H object.
	 * @return
	 */
	string getFilename();

	void save(const string fileName);
};

#endif /* ISEA3H_H */
