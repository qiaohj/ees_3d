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
	return values[p_id];
}

void ISEA3H::save(const string fileName) {
	vector<string> output;
	char line[50];
	// Note: The old version has only 5 columns without lon and lat columns.
	sprintf(line, "global_id,v");
	output.push_back(line);
	for (auto item : values) {
		char line[50];
		sprintf(line, "%u,%f", item.first, item.second);
		output.push_back(line);
	}

	if (output.size() > 0) {
		CommonFun::writeFile(output, fileName.c_str());
		output.clear();
	}
}
ISEA3H::~ISEA3H() {

}

