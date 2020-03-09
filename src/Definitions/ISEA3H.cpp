/**
 * @file ISEA3H.cpp
 * @brief Class ISEA3H. A class to handle the raster file
 * @author Huijie Qiao
 * @version 1.0
 * @date 11/25/2018
 * @details
 * Copyright 2014-2019 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 */

#include "ISEA3H.h"
#include <iostream>
#include <fstream>
#include "../Universal/const.h"
#include "../Universal/easylogging.h"
ISEA3H::ISEA3H() {

}
ISEA3H::ISEA3H(boost::unordered_map<int, float> p_values){
    values = p_values;
}

int ISEA3H::getCellSize() {
	return values.size();
}

void ISEA3H::setValue(int p_id, float p_value) {
	values[p_id] = p_value;
}

boost::unordered_map<int, float> ISEA3H::getValues() {
	return values;
}

int* ISEA3H::getIDs() {
	int *array = new int[values.size()];
	int i = 0;
	for (auto iitem : values) {
		int id = iitem.first;
		array[i++] = id;
	}
	return array;
}
float ISEA3H::readByID(int p_id) {
    if (values.find(p_id)==values.end()){
        return ((float)NODATA);
    }else{
        return values[p_id];
    }
}

void ISEA3H::save(const string fileName) {

}
ISEA3H::~ISEA3H() {

}

