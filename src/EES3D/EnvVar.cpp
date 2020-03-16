/**
 * @file EnvVar.cpp
 * @brief Class EnvVar. A class to handle the environmental layers in ISEA format
 * @author Huijie Qiao
 * @version 1.0
 * @date 3/3/2020
 * @details
 * Copyright 2014-2020 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 */

#include "EnvVar.h"

EnvVar::EnvVar(){
    layers = new unordered_map<int, ISEA*>();
}
EnvVar::EnvVar(string p_env_name, sqlite3 *p_env_db, vector<int> *timeLine) {
    layers = new unordered_map<int, ISEA*>();
    envName = p_env_name;
    Utility::readEnvInfo(p_env_db, p_env_name, true, layers);
}

ISEA* EnvVar::getValues(int p_year_i) {
    return layers->at(p_year_i);
}

float EnvVar::readByID(int p_year_i, int p_id) {
    float value = layers->at(p_year_i)->readByID(p_id);
    return value;
}

EnvVar::~EnvVar() {
    for (auto it : *layers){
        delete it.second;
    }
    delete layers;
}

