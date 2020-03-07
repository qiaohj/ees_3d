/**
 * @file Simulation3D.h
 * @brief Class Simulation3D. A class to handle the attributes and behaviors of a simulation
 * @author Huijie Qiao
 * @version 1.0
 * @date 3/7/2020
 * @details
 * Copyright 2014-2020 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 */


#ifndef DEFINITIONS_SIMULATION3D_H_
#define DEFINITIONS_SIMULATION3D_H_
using namespace std;
#include <string>
#include <vector>
#include <sqlite3.h>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include "../JsonPaster/include/json/json.h"
#include "NicheBreadth.h"
#include "../Universal/easylogging.h"
#include "../Universal/CommonFun.h"
#include "DBField.h"
#include "SpeciesObject3D.h"
#include "EnvironmentalISEA3H.h"
#include "ISEA3H.h"
#include "IndividualOrganism3D.h"
#include "Neighbor3D.h"
/**
 * @brief A class to handle the attributes and behaviors of a virtual species
 */
class Simulation3D {
private:
    vector<SpeciesObject3D*> species;
    vector<string> environment_labels;
    boost::unordered_map<string, EnvironmentalISEA3H*> environments;
    vector<int> timeLine;
    ISEA3H* mask;
    string targetFolder;
    string logFile;
    sqlite3* log_db;
    int total_years;
    bool overwrite;
    Neighbor3D* neighborInfo;
    unsigned long memLimit;
    boost::unordered_map<int, boost::unordered_map<SpeciesObject3D*, boost::unordered_map<int, vector<IndividualOrganism3D*> > > > all_individualOrganisms;
public:
    /**
     * @brief Constructor of Simulation3D
     * @param json_path the path to configuration file in JSON format
     */
    Simulation3D();
    void setNeighbor(Neighbor3D *neighborInfo);
    void setOverwrite(bool p_overwrite);
    void setMemLimit(unsigned long memLimit);
    bool getOverwrite();
    void saveGroupmap(int year, boost::unordered_map<SpeciesObject3D*, ISEA3H*> species_group_maps);
    void setTotalYears(int p_total_years);
    int getTotalYears();
    int run();
    void addSpecies(SpeciesObject3D *species);
    void addEnvironmentLabel(string environment_labels);
    void addEnvironment(string environment_label, EnvironmentalISEA3H *env);
    void setTimeLine(int from, int to, int step);
    void setMask(ISEA3H* p_mask);
    void init();
    void setTargetFolder(string p_target);
    vector<SpeciesObject3D*> getSpecies();
    vector<string> getEnvironmentLabels();
    EnvironmentalISEA3H* getEnvironment(string environment_label);
    vector<int> getTimeLine();
    ISEA3H* getMask();
    string getTargetFolder();
    void createLogDB();
    set<int> getDispersalMap_2(IndividualOrganism3D *individualOrganism, int year);
    void cleanEnvironments();
    void cleanActivedIndividualOrganism3Ds();
    void cleanSpecies();
    boost::unordered_map<string, ISEA3H*> getEnvironmenMap(int p_year);
    IndividualOrganism3D* getUnmarkedOrganism(boost::unordered_map<int, vector<IndividualOrganism3D*> > *organisms);
    void markJointOrganism(int short p_group_id, IndividualOrganism3D *p_unmarked_organism, boost::unordered_map<int, vector<IndividualOrganism3D*> > *organisms);
    set<int> getNeighbors(int id, int distance);
    int getDividedYear(IndividualOrganism3D *o_1, IndividualOrganism3D *o_2);
    int getMinDividedYear(int speciation_year, int short group_id_1, int short group_id_2, boost::unordered_map<int, vector<IndividualOrganism3D*> > *organisms, int current_year);
    int distance3D(int id1, int id2, int limited);
    void markedSpeciesID(int short group_id, int short temp_species_id, boost::unordered_map<int, vector<IndividualOrganism3D*> > *organisms);
    int short getTempSpeciesID(int short group_id, boost::unordered_map<int, vector<IndividualOrganism3D*> > *organisms);
    void generateSpeciationInfo(int year, bool is_tree);
    virtual ~Simulation3D();



};

#endif /* DEFINITIONS_SIMULATION3D_H_ */