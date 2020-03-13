/**
 * @file Utility.h
 * @brief Class Utility. A class to implement the functions used in the multi-object.
 * @author Huijie Qiao
 * @version 1.0
 * @date 3/13/2020
 * @details
 * Copyright 2014-2020 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 **/
#ifndef UTILITY_
#define UTILITY_

using namespace std;
#include <sqlite3.h>
#include <boost/unordered_map.hpp>
#include "../Universal/easylogging.h"
#include "DBField.h"

class Utility {
public:
    /**
     * @brief load the env information from db
     * @param s the string array contains the SQL commend
     * @param db the database to execute the command
     * @param year the year to read, -1 means all data.
     */
    static void readEnvInfo(sqlite3 *db, string tablename, bool with_year,
            boost::unordered_map<int, boost::unordered_map<int, float>*> *&values);
};
#endif /* UTILITY_ */
