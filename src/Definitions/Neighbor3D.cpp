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

Neighbor3D::Neighbor3D(const string p_filename) {
	filename = p_filename;

	CSVReader reader(filename, " ", true);
	vector<vector<string> > data = reader.getData();
	for (vector<string> vec : data) {
		bool isFirst = true;
		int key = NODATA;
		set<int> values;
		for (string data : vec) {
			if (isFirst) {
				key = stoi(data);
				isFirst = false;
			} else {
				values.insert(stoi(data));
			}
		}
		neighbors[key] = values;

	}

}
void Neighbor3D::getNeighborByID(unsigned p_id, unsigned distance, set<int>* neighbors) {

	if (distance==0){
		neighbors->insert(p_id);
	}else{
		set<int> v = neighbors[p_id];
		for (int nei_id : v){
			if (find(neighbors->begin(), neighbors->end(), nei_id) == neighbors->end()){
				neighbors->insert(nei_id);
				getNeighborByID(nei_id, distance - 1, neighbors);
			}
		}
	}
}
unsigned Neighbor3D::distance(unsigned p_id1, unsigned p_id2, set<int>* neighbors){
	//to do:
	return 0;
}
string Neighbor3D::getFilename() {
	return filename;
}

Neighbor3D::~Neighbor3D() {

}

