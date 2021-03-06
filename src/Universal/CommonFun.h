/**
 * @file CommonFun.h
 * @brief Class CommonFun. A class to implement the public functions common used in the application.
 * @author Huijie Qiao
 * @version 1.0
 * @date 3/13/2020
 * @details
 * Copyright 2014-2020 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 */

#ifndef CommonFun_H
#define CommonFun_H


#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <float.h>
#include <math.h>
#include <unistd.h>
#include <sqlite3.h>
#include <cmath>
#include <sys/resource.h>

#include <boost/algorithm/string/join.hpp>
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>

#include "Const.h"
#include "easylogging.h"

using namespace std;

#ifndef M_PI
#define M_PI    3.1415926535897932384626433832795
#endif

#ifndef earth_radius_km
#define earth_radius_km    6371.0
#endif


typedef struct {
        uint32_t virtualMem;
        uint32_t physicalMem;
    } processMem_t;
class CommonFun {

public:
    static processMem_t GetProcessMemory();
    static int parseLine(char *line);
    static string quoteSql(string *s);
    static bool checkKey();
    static int callback(void *NotUsed, int argc, char **argv, char **azColName);
    /**
     * @brief read the text file to a string
     * @param path the path to the text file
     * @return
     */
    static string readFile(const char *path);
    /**
     * @brief split string with a given delimiter
     * @param the string to split
     * @param delimiter
     * @return
     */
    static vector<string> splitStr(string s, string delimiter);

    static bool fileExist (const string& name);

    /**
     * @brief remove the the given extension from a file name
     * @param path
     * @param extension
     * @return
     */
    static string removeSuffix(const string &path,
            const string &extension);
    /**
     * @brief output a string to a text file
     * @param s the string to save
     * @param path the path to the text file
     */
    static void writeFile(string s, const char *path);
    static bool between(int v, int x, int y);
    /**
     * @brief create a folder
     * @param path the folder's path
     */
    static void createFolder(const char *path);
    /**
     * @brief remove the give file.
     * @param path
     */
    static void deleteFile(const char *path);
    /**
     * @brief output a string array to a text file
     * @param s the string array to save
     * @param path the path to the text file
     */
    static void writeFile(vector<string> s, const char *path);
    //static void writeFile(set<string> s, const char *path);
    /**
     * @brief execute a sql command
     * @param s the string array contains the SQL commend
     * @param db the database to execute the command
     */
    static void executeSQL(vector<string> s, sqlite3 *db, bool output);
    /**
     * @brief execute a sql command
     * @param s the string contains the SQL commend
     * @param db the database to execute the command
     */
    static void executeSQL(string s, sqlite3 *db, bool output);
    /**
     * @brief add leading ZERO to a number and convert the number to a string with fixed length
     * @param value the number to convert
     * @param digits the length of the string
     * @return
     */
    static string fixedLength(int value, int digits);
    /**
     * @brief returns the peak (maximum so far) resident set size (physical * memory use) measured in bytes, or zero if the value cannot be determined on this OS.
     * @return
     */
    static size_t getPeakRSS();
    /**
     * @breif return the current used memory
     * @return
     */
    static size_t getCurrentRSS(int unit);
    /**
     * @brief output the current memory usage for the application
     * @param line
     * @param is
     * @param last
     * @return
     */
    static size_t writeMemoryUsage(unsigned line, bool is, size_t last);

    /**
     * @brief calculate the Euclidean distance between to points.
     * @param x1
     * @param y1
     * @param x2
     * @param y2
     * @return
     */
    template<typename T> static double EuclideanDistance(T x1, T y1, T x2,
            T y2);
    /**
     * @brief in C++, we can't compare the equivalence of two float/double variables. this function is used to detect the equivalence of two float/double variables under a given precision.
     * @param a
     * @param b
     * @return
     */
    template<typename T> static bool AlmostEqualRelative(T a, T b);
    /**
     * @brief convert any type to a string
     * @param t
     * @return
     */
    template<typename T> static string to_str(const T &t);
    /**
     * @brief clear a vector and release the memory
     * @param v
     */

    /**
     * @brief clear a hash map and release the memory
     * @param v
     */
    template<typename T> static void clearVectorObj(T *v);
    template<typename T> static void clearUnorderedMapObjInt(T *v);
    template<typename T> static void clearUnorderedMapObjString(T *v);
    template<typename T> static void clearUnorderedMapKey(T *v);
    template<typename T> static void freeContainer(T& p_container);
    static void clearUnorderedMap(vector<string> *v);

};
template<typename T> void CommonFun::freeContainer(T &p_container) {
    T empty;
    swap(p_container, empty);
}
template<typename T> void CommonFun::clearUnorderedMapObjString(T *v) {
    vector<string> erased_key;
    for (auto it : *v) {
        erased_key.push_back(it.first);
    }
    for (auto key : erased_key) {
        if ((*v)[key] != NULL) {
            delete (*v)[key];
        }
        v->erase(key);
    }
    freeContainer(erased_key);
    freeContainer(v);
}
template<typename T> void CommonFun::clearUnorderedMapObjInt(T *v) {
    vector<int> erased_key;
    for (auto it : *v) {
        erased_key.push_back(it.first);
    }
    for (auto key : erased_key) {
        if ((*v)[key] != NULL) {
            delete (*v)[key];
        }
        v->erase(key);
    }
    freeContainer(erased_key);
    freeContainer(v);
}
template<typename T> void CommonFun::clearUnorderedMapKey(T *v) {
    for (auto it : *v) {
        delete it;
    }
}

template<typename T> void CommonFun::clearVectorObj(T *v) {
    for (typename T::iterator it = v->begin(); it != v->end(); ++it) {
        delete *it;
        *it = NULL;
    }
    v->clear();
    T().swap(*v);
}
template<typename T> double CommonFun::EuclideanDistance(T x1, T y1, T x2, T y2) {
    T x = x1 - x2;
    T y = y1 - y2;
    double dist;
    dist = pow(x, 2) + pow(y, 2);
    dist = sqrt(dist);
    return dist;
}

template<typename T> bool CommonFun::AlmostEqualRelative(T a, T b) {
    return fabs(a - b) < FLT_EPSILON;
}
template<typename T> string CommonFun::to_str(const T &t) {
    ostringstream os;
    os << t;
    return os.str();
}
#endif /* CommonFun_H */
