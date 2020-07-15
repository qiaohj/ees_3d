/**
 * @file CommonFun.cpp
 * @brief Class CommonFun. A class to implement the public functions commonly used in the application
 * @author Huijie Qiao
 * @version 1.0
 * @date 3/13/2020
 * @details
 * Copyright 2014-2020 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 */

#include "CommonFun.h"
//static const double DEG_TO_RAD = 0.017453292519943295769236907684886;


/**
 * @brief A class to implement the public functions common used in the application.
 */
int CommonFun::callback(void *NotUsed, int argc, char **argv,
        char **azColName) {
    int i;
    for (i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}


bool CommonFun::between(int v, int x, int y) {
	if ((v >= x) && (v <= y)) {
		return true;
	}
	if ((v >= y) && (v <= x)) {
		return true;
	}
	return false;
}

string CommonFun::readFile(const char *path) {
    FILE *file = fopen(path, "rb");
    if (!file)
        return string("");
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    string text;
    char *buffer = new char[size + 1];
    buffer[size] = 0;
    if (fread(buffer, 1, size, file) == (unsigned long) size)
        text = buffer;
    fclose(file);
    delete[] buffer;
    return text;
}

string CommonFun::removeSuffix(const string &path, const string &extension) {
    if (extension.length() >= path.length())
        return string("");
    string suffix = path.substr(path.length() - extension.length());
    if (suffix != extension)
        return string("");
    return path.substr(0, path.length() - extension.length());
}
void CommonFun::writeFile(string s, const char *path) {
    ofstream outfile(path);
    if (!outfile.is_open()) {
        cerr << "Couldn't open " << path << endl;
    }
    outfile << s << endl;
    outfile.close();
}
/*
void CommonFun::writeFile(set<string> s, const char *path) {
    string joined = boost::algorithm::join(s, "\n");
    writeFile(joined, path);
}
*/
void CommonFun::writeFile(vector<string> s, const char *path) {
    //string joined = boost::algorithm::join(s, "\n");
    //writeFile(joined, path);
    ofstream output_file(path);
    // the important part
    for (const auto &e : s)
        output_file << e << "\n";
}
void CommonFun::executeSQL(vector<string> s, sqlite3 *db, bool output) {
    string joined = boost::algorithm::join(s, " ");
    CommonFun::executeSQL(joined, db, output);
}
processMem_t CommonFun::GetProcessMemory() {
    FILE *file = fopen("/proc/self/status", "r");
    char line[128];
    processMem_t processMem;

    while (fgets(line, 128, file) != NULL) {
        if (strncmp(line, "VmSize:", 7) == 0) {
            processMem.virtualMem = parseLine(line);
        }

        if (strncmp(line, "VmRSS:", 6) == 0) {
            processMem.physicalMem = parseLine(line);
        }
        if (processMem.physicalMem != 0 && processMem.virtualMem != 0) {
            break;
        }
    }
    fclose(file);
    return processMem;
}
int CommonFun::parseLine(char *line) {
    // This assumes that a digit will be found and the line ends in " Kb".
    int i = strlen(line);
    const char *p = line;
    while (*p < '0' || *p > '9') p++;
    line[i - 3] = '\0';
    i = atoi(p);
    return i;
}
string CommonFun::quoteSql(string *s) {
    return string("'") + (*s) + string("'");
}
void CommonFun::executeSQL(string s, sqlite3 *db, bool output) {
    char *zErr;
    string data("CALLBACK FUNCTION");
    //LOG(DEBUG) << "Query: "<< s;
    int rc = sqlite3_exec(db, s.c_str(), callback, (void*)data.c_str() , &zErr);

    if (rc != SQLITE_OK) {
        if (zErr != NULL) {
            LOG(ERROR) << "SQL error: " << zErr;
            if (output){
                LOG(ERROR) << "SQL is : " << s;
            }
            sqlite3_free(zErr);
        }
    } else {
        //LOG(INFO) << "call back: "<< data;
        //LOG(INFO)<<"success insert ";
    }
}

void CommonFun::createFolder(const char *path) {
    if (!(boost::filesystem::exists(path))) {
        boost::filesystem::create_directory(path);
    }
}
void CommonFun::deleteFile(const char *path) {
    if (boost::filesystem::exists(path)) {
        boost::filesystem::remove(path);
    }
}

string CommonFun::fixedLength(int value, int digits = 3) {
    unsigned int uvalue = value;
    if (value < 0) {
        uvalue = -uvalue;
    }
    string result;
    while (digits-- > 0) {
        result += ('0' + uvalue % 10);
        uvalue /= 10;
    }
    if (value < 0) {
        result += '-';
    }
    reverse(result.begin(), result.end());
    return result;
}
bool CommonFun::fileExist(const string &name) {
    return (access(name.c_str(), F_OK) != -1);
}
vector<string> CommonFun::splitStr(string s, string delimiter){
    vector<string> v;
    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        v.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    v.push_back(s);
    return v;
}
/**
 * Returns the peak (maximum so far) resident set size (physical
 * memory use) measured in bytes, or zero if the value cannot be
 * determined on this OS.
 */
size_t CommonFun::getPeakRSS() {
    struct rusage rusage;
    getrusage( RUSAGE_SELF, &rusage);
    return (size_t) (rusage.ru_maxrss * 1024L);
}

/**
 * Returns the current resident set size (physical memory use) measured
 * in bytes, or zero if the value cannot be determined on this OS.
 */
size_t CommonFun::getCurrentRSS(int unit) {
    long rss = 0L;
    FILE *fp = NULL;
    if ((fp = fopen("/proc/self/statm", "r")) == NULL)
        return (size_t) 0L; /* Can't open? */
    if (fscanf(fp, "%*s%ld", &rss) != 1) {
        fclose(fp);
        return (size_t) 0L; /* Can't read? */
    }
    fclose(fp);
    return (size_t) rss * (size_t) sysconf( _SC_PAGESIZE) / unit;

}

size_t CommonFun::writeMemoryUsage(unsigned line, bool is, size_t last) {
    if (is) {
        boost::this_thread::sleep(boost::posix_time::milliseconds(1));
        size_t current = getCurrentRSS(1024*1024);
        if ((current - last) != 0) {
            LOG(INFO) << "Memory usage " << fixedLength(line, 3) << ": "
                    << (current - last);
        }
        return current;
    }
    return 0;
}
