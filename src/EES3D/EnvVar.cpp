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

}
EnvVar::EnvVar(string p_env_name, sqlite3 *p_env_db) {
    envName = p_env_name;
    Utility::readEnvInfo(p_env_db, p_env_name, true, layers);
}

ISEA* EnvVar::getValues(int p_year) {
    /*
    LOG(DEBUG)<<"year is "<<p_year;
    for (auto it : *layers){
        LOG(DEBUG)<<it.first<<" in the list";
    }*/
    return layers[p_year];
}

float EnvVar::readByID(int p_year, int p_id) {
    float value = layers[p_year]->readByID(p_id);
    return value;
}

EnvVar::~EnvVar() {
    for (auto it : layers){
        delete it.second;
    }
}

