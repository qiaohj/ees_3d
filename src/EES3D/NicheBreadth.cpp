/**
 * @file NicheBreadth.cpp
 * @brief Class NicheBreadth. A class of the niche breadth of a virtual species
 * @author Huijie Qiao
 * @version 1.0
 * @date 3/13/2020
 * @details
 * Copyright 2014-2020 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 */

#include "NicheBreadth.h"

NicheBreadth::NicheBreadth(double p_min, double p_max) {
    min = p_min;
    max = p_max;
}

NicheBreadth::~NicheBreadth() {

}
double NicheBreadth::getMin(){
    return min;
}
double NicheBreadth::getMax(){
    return max;
}

