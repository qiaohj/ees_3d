/**
 * @file ISEA.cpp
 * @brief Class ISEA. A class to handle the raster file
 * @author Huijie Qiao
 * @version 1.0
 * @date 3/13/2020
 * @details
 * Copyright 2014-2020 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 */

#include "ISEA.h"

ISEA::ISEA() {

}
ISEA::ISEA(unordered_map<int, double> &p_values){
    values = p_values;
}

int ISEA::getCellSize() {
	return values.size();
}

void ISEA::setValue(int p_id, double p_value) {
    values[p_id] = p_value;
}

unordered_map<int, double> ISEA::getValues() {
    return values;
}

double ISEA::readByID(int p_id) {
    if (values.find(p_id)==values.end()){
        return ((float)NODATA);
    }else{
        return values[p_id];
    }
}

ISEA::~ISEA() {
    //LOG(DEBUG)<<"I DO";
    values.clear();
    unordered_map<int, double>().swap(values);
}

