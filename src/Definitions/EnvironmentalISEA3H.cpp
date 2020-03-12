/**
 * @file EnvironmentalISEA3H.cpp
 * @brief Class EnvironmentalISEA3H. A class to handle the environmental layers in ISEA3H format
 * @author Huijie Qiao
 * @version 1.0
 * @date 3/3/2020
 * @details
 * Copyright 2014-2020 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 */

#include "EnvironmentalISEA3H.h"
EnvironmentalISEA3H::EnvironmentalISEA3H(string p_env_name, sqlite3 *p_env_db, vector<int> timeLine) {
    envName = p_env_name;
    boost::unordered_map<int, boost::unordered_map<int, float>> values = CommonFun::readEnvInfo(p_env_db, p_env_name, true);
    for (unsigned year_i = 0; year_i<timeLine.size() ; year_i++) {
        ISEA3H *v = new ISEA3H(values[timeLine[year_i]]);
        //LOG(DEBUG)<<"Initial environments information size is "<<values[y].size()<<" to key "<<key;
        layers[year_i] = v;
    }
}

ISEA3H* EnvironmentalISEA3H::getValues(int p_year_i) {
    //LOG(DEBUG) << "Layer name is " << envName << ". Layer Size is " << layers.size();
    return layers[p_year_i];
}

float EnvironmentalISEA3H::readByID(int p_year_i, int p_id) {

    float value = layers[p_year_i]->readByID(p_id);
    return value;
}

EnvironmentalISEA3H::~EnvironmentalISEA3H() {
    for (auto it : layers){
        delete it.second;
    }
}

