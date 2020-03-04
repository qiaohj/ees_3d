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
#include "../Universal/log.hpp"
ISEA3H::ISEA3H() {

}
ISEA3H::ISEA3H(const string p_filename) {

	filename = p_filename;
	//LOG(INFO)<<"Reading "<<filename;
	CSVReader reader(filename, " ", true);
	vector<vector<string> > data = reader.getData();
	for (vector<string> vec : data) {
		bool isFirst = true;
		int key = NODATA;
		float value = NODATA;
		for (string data : vec) {
			if (isFirst) {
				key = stoi(data);
				isFirst = false;
			} else {
				value = std::stof(data);
				isFirst = true;
			}
		}
		values[key] = value;

	}

}
unsigned ISEA3H::getCellSize() {
	return values.size();
}
string ISEA3H::getFilename() {
	return filename;
}
void ISEA3H::setValue(unsigned p_id, float p_value) {
	values[p_id] = p_value;
}

boost::unordered_map<unsigned, float> ISEA3H::getValues() {
	return values;
}

unsigned* ISEA3H::getIDs() {
	unsigned *array = new unsigned[values.size()];
	int i = 0;
	for (auto iitem : values) {
		unsigned id = iitem.first;
		array[i++] = id;
	}
	return array;
}
float ISEA3H::readByID(unsigned p_id) {
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

