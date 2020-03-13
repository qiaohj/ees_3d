/**
 * @file ISEA3H.cpp
 * @brief Class ISEA3H. A class to handle the raster file
 * @author Huijie Qiao
 * @version 1.0
 * @date 3/13/2020
 * @details
 * Copyright 2014-2020 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 */

#include "ISEA3H.h"

ISEA3H::ISEA3H() {
    values = new boost::unordered_map<int, float>();
}
ISEA3H::ISEA3H(boost::unordered_map<int, float> *p_values){
    values = p_values;
}

int ISEA3H::getCellSize() {
	return values->size();
}

void ISEA3H::setValue(int p_id, float p_value) {
    values->erase(p_id);
    values->insert({p_id, p_value});
}

boost::unordered_map<int, float>* ISEA3H::getValues() {
	return values;
}

float ISEA3H::readByID(int p_id) {
    if (values->find(p_id)==values->end()){
        return ((float)NODATA);
    }else{
        return values->at(p_id);
    }
}

ISEA3H::~ISEA3H() {
    LOG(DEBUG)<<"7 "<<CommonFun::getCurrentRSS(1);
    boost::unordered_map<int, float>().swap(*values);
    delete values;
    LOG(DEBUG)<<"7.2 "<<CommonFun::getCurrentRSS(1);
    values = NULL;
    LOG(DEBUG)<<"7.5 "<<CommonFun::getCurrentRSS(1);
}

