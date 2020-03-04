/*
 * CSVReadere.h
 *
 *  Created on: Mar 3, 2020
 *      Author: huijieqiao
 */

#ifndef CSVREADERE_H_
#define CSVREADERE_H_

using namespace std;

#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <string>
#include <algorithm>
#include <boost/algorithm/string.hpp>

/*
 * A class to read data from a csv file.
 */
class CSVReader {
	string fileName;
	string delimeter;
	bool skipFirst;
public:
	CSVReader(string filename, string delm, bool skip_first);
// Function to fetch data from a CSV File
	vector<vector<string> > getData();
};


#endif /* CSVREADERE_H_ */
