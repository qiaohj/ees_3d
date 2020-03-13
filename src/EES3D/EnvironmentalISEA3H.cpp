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

EnvironmentalISEA3H::EnvironmentalISEA3H(){
    layers = new boost::unordered_map<int, ISEA3H*>();
}
EnvironmentalISEA3H::EnvironmentalISEA3H(string p_env_name, sqlite3 *p_env_db, vector<int> &timeLine) {
    layers = new boost::unordered_map<int, ISEA3H*>();
    envName = p_env_name;
    boost::unordered_map<int, boost::unordered_map<int, float>*> *values = new boost::unordered_map<int, boost::unordered_map<int, float>*>();
    Utility::readEnvInfo(p_env_db, p_env_name, true, values);
    for (unsigned year_i = 0; year_i<timeLine.size() ; year_i++) {
        ISEA3H *v = new ISEA3H(values->at(timeLine[year_i]));
        //LOG(DEBUG)<<"Initial environments information size is "<<values[y].size()<<" to key "<<key;
        layers->insert({year_i, v});
    }
    delete values;
}

ISEA3H* EnvironmentalISEA3H::getValues(int p_year_i) {
    return layers->at(p_year_i);
}

float EnvironmentalISEA3H::readByID(int p_year_i, int p_id) {

    float value = layers->at(p_year_i)->readByID(p_id);
    return value;
}

EnvironmentalISEA3H::~EnvironmentalISEA3H() {
    LOG(DEBUG)<<"5 "<<CommonFun::getCurrentRSS(1);
    for (auto it : *layers){
        LOG(DEBUG)<<"6 "<<CommonFun::getCurrentRSS(1);
        delete it.second;
        LOG(DEBUG)<<"6.5 "<<CommonFun::getCurrentRSS(1);
    }
    delete layers;
}

