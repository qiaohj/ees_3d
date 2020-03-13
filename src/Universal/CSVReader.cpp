/**
 * @file CSVReader.cpp
 * @brief Class CSVReader. A class to read the data from CSV file
 * @author Huijie Qiao
 * @version 1.0
 * @date 3/13/2020
 * @details
 * Copyright 2014-2020 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 */

#include "CSVReader.h"

CSVReader::CSVReader(string filename, string delm = " ", bool skip_first = false){
	fileName = filename;
	delimeter = delm;
	skipFirst = skip_first;
}
/*
 * Parses through csv file line by line and returns the data
 * in vector of vector of strings.
 */
vector<vector<string> > CSVReader::getData() {
	ifstream file(fileName);
	vector<vector<string> > dataList;
	string line = "";
// Iterate through each line and split the content using delimeter
	while (getline(file, line)) {
		if (skipFirst){
			skipFirst = false;
			continue;
		}
		vector<string> vec;
		boost::algorithm::split(vec, line, boost::is_any_of(delimeter));
		dataList.push_back(vec);
	}
// Close the File
	file.close();
	return dataList;
}
/**
int main() {
// Creating an object of CSVWriter
	CSVReader reader("example.csv");
// Get the data from CSV File
	vector<vector<string> > dataList = reader.getData();
// Print the content of row by row on screen
	for (vector<string> vec : dataList) {
		for (string data : vec) {
			cout << data << " , ";
		}
		cout << endl;
	}
	return 0;
}
**/
