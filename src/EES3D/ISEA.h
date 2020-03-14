/**
 * @file ISEA.h
 * @brief Class ISEA. A class to handle the ISEA format file
 * @author Huijie Qiao
 * @version 1.0
 * @date 3/3/2020
 * @details
 * Copyright 2014-2020 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 */

#ifndef ISEA_H
#define ISEA_H

#include <string>
#include <iostream>
#include <fstream>
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/io.hpp>
//#include <boost/unordered_map.hpp>
#include "../Universal/Const.h"
#include "../Universal/CommonFun.h"
#include "../Universal/CSVReader.h"
#include "../Universal/easylogging.h"
/**
 *@brief A class to handle the ISEA data
 */
class ISEA {
private:
	unordered_map<int, float> *values;
public:
	/**
	 * @brief Constructor of ISEA class No.2 (Create a blank map)
	 */
	ISEA();

	ISEA(unordered_map<int, float> *p_values);
	/**
	 * @brief Constructor of ISEA class No.1 (load an ISEA from a file)
	 * @param p_filename the file to load the map info
	 */
	ISEA(const string p_filename);

	/**
	 * @brief Destructor of ISEA class
	 *
	 * release all the resources
	 */
	virtual ~ISEA();

	/**
	 * @brief read value by ID of face
	 * @return
	 */
	float readByID(int p_id);

	/**
	 * @brief return the number of faces.
	 * @return
	 */
	int getCellSize();


	/**
	 * @brief set value to a given face by id.
	 * @return
	 */
	void setValue(int p_id, float p_value);
	/**
	 * @brief return the face/value map.
	 * @return
	 */
	unordered_map<int, float>* getValues();

};

#endif /* ISEA_H */
