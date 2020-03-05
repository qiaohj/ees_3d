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
EnvironmentalISEA3H::EnvironmentalISEA3H(const string p_env_name,
        sqlite3 *p_env_db, unsigned p_burn_in_year, unsigned p_begin_year,
        unsigned p_end_year, unsigned p_step) {
    boost::unordered_map<unsigned, boost::unordered_map<unsigned, float>> values = CommonFun::readEnvInfo(
            p_env_db, p_env_name, true);
    for (unsigned y = p_begin_year + p_burn_in_year; y >= p_end_year; y -=
            p_step) {
        unsigned key = p_begin_year + p_burn_in_year - y;
        ISEA3H *v = new ISEA3H(values[y]);
        //LOG(INFO)<<"Initial environments information size is "<<values[y].size()<<" to key "<<key;
        layers[key] = v;
        if (y == 0) {
            break;
        }
    }
    this->burnInYears = p_burn_in_year;
}
EnvironmentalISEA3H::EnvironmentalISEA3H(const string p_basefolder,
        unsigned p_burn_in_year, unsigned p_begin_year, unsigned p_end_year,
        unsigned p_step) {
    for (unsigned y = p_begin_year + p_burn_in_year; y >= p_end_year; y -=
            p_step) {
        unsigned year = y / 100;
        string layer = p_basefolder + "/" + CommonFun::fixedLength(year, 4)
                + ".csv";
        if (y > p_begin_year) {
            layer = p_basefolder + "/"
                    + CommonFun::fixedLength(p_begin_year / 100, 4) + ".csv";
        }

        //RasterObject* r = new RasterObject(layer);
        unsigned key = p_begin_year + p_burn_in_year - y;
        ISEA3H *v = new ISEA3H(layer);
        //LOG(INFO)<<"Initial environments information from "<<layer<<" to key "<<key;
        layers[key] = v;
        if (y == 0) {
            break;
        }
    }
    this->burnInYears = p_burn_in_year;
}
unsigned EnvironmentalISEA3H::getBurnInYears() {
    return burnInYears;
}
ISEA3H* EnvironmentalISEA3H::getValues(unsigned p_year) {
    //LOG(INFO)<<"get:"<<p_year;
    //LOG(INFO)<<" from "<<layers[p_year]->getFilename();

    return layers[p_year];
}

float EnvironmentalISEA3H::readByID(unsigned p_year, unsigned p_id) {

    float value = layers[p_year]->readByID(p_id);
    return value;
}

EnvironmentalISEA3H::~EnvironmentalISEA3H() {

}

