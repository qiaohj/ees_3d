/**
 * @file ISEA_MEMTEST.cpp
 * @brief Class ISEA_MEMTEST. A class to handle the raster file
 * @author Huijie Qiao
 * @version 1.0
 * @date 3/13/2020
 * @details
 * Copyright 2014-2020 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 */

#include "ISEA_MEMTEST.h"

ISEA_MEMTEST::ISEA_MEMTEST() {
    values = new unordered_map<int, float>();
}
ISEA_MEMTEST::ISEA_MEMTEST(unordered_map<int, float> *p_values){
    values = p_values;
}

int ISEA_MEMTEST::getCellSize() {
	return values->size();
}

void ISEA_MEMTEST::setValue(int p_id, float p_value) {
    values->erase(p_id);
    values->insert({p_id, p_value});
}

unordered_map<int, float>* ISEA_MEMTEST::getValues() {
	return values;
}

float ISEA_MEMTEST::readByID(int p_id) {
    if (values->find(p_id)==values->end()){
        return ((float)NODATA);
    }else{
        return values->at(p_id);
    }
}

ISEA_MEMTEST::~ISEA_MEMTEST() {

    //values->clear();
    LOG(DEBUG)<<"1: "<<CommonFun::getCurrentRSS(1);

    //unordered_map<int, float>(*values).swap(*values);
    CommonFun::freeContainerRemoved(values);
    LOG(DEBUG)<<"2: "<<CommonFun::getCurrentRSS(1);
    //delete values;
    LOG(DEBUG)<<"3: "<<CommonFun::getCurrentRSS(1);

    //values = NULL;
}

