/**
 * @file Simulation.h
 * @brief Class Simulation. A class to handle the attributes and behaviors of a simulation
 * @author Huijie Qiao
 * @version 1.0
 * @date 3/7/2020
 * @details
 * Copyright 2014-2020 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 */


#ifndef DEFINITIONS_SIMULATION_H_
#define DEFINITIONS_SIMULATION_H_

using namespace std;
#include <string>
#include <vector>
#include <sqlite3.h>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include "NicheBreadth.h"
#include "../Universal/easylogging.h"
#include "../Universal/CommonFun.h"
#include "DBField.h"
#include "Species.h"
#include "EnvVar.h"
#include "ISEA.h"
#include "Organism.h"
#include "Neighbor.h"
#include "Utility.h"

/**
 * @brief A class to handle the attributes and behaviors of a virtual species
 */
class Simulation {
private:
    vector<string> logs;
    unordered_map<string, Species*> species;
    vector<string> environment_labels;
    unordered_map<string, EnvVar*> *environments;
    vector<int> timeLine;
    ISEA* mask;
    string mask_table;
    string targetFolder;
    string logFile;
    sqlite3* log_db;
    bool overwrite;
    int burnInYear;
    string label;
    Neighbor* neighborInfo;
    unsigned long memLimit;
    int indexSimulation;
    int totalSimulation;
    unordered_map<int, unordered_map<Species*, unordered_map<int, vector<Organism*> > > > all_organisms;

public:
    /**
     * @brief Constructor of Simulation
     */
    Simulation(Species *species, string label, int burnInYear, string target, bool p_overwrite, unsigned long memLimit,
            vector<int>& p_timeLine, Neighbor* neighborInfo, vector<string> environment_labels, string mask_table);
    void setIndexSimulation(int indexSimulation);
    void setTotalSimulation(int totalSimulation);
    void commitLog();
    void setNeighbor(Neighbor *neighborInfo);
    void generateSuitable();
    bool getOverwrite();
    void saveGroupmap(int year_i, unordered_map<Species*, ISEA*> *species_group_maps);
    int run();
    Species *ancestor;
    void addEnvironment(string environment_label, EnvVar *env);
    void setMask(ISEA* p_mask);
    bool init(unordered_map<string, EnvVar*>* environments_base, sqlite3* env_db, unordered_map<string, ISEA*>* masks);
    void setTargetFolder(string p_target);
    vector<string> getEnvironmentLabels();
    EnvVar* getEnvironment(string environment_label);
    vector<int> getTimeLine();
    ISEA* getMask();
    string getTargetFolder();
    void createLogDB();
    set<int> getDispersalMap_2(Organism *organism);
    void getEnvironmentMap(int p_year_i, unordered_map<string, ISEA*> *maps);
    int getUnmarkedID(unordered_map<int, vector<Organism*> > *organisms);
    void markJointOrganism(int p_group_id, int unmarked_id, int dispersal_ability, unordered_map<int, vector<Organism*> > *organisms);
    set<int> getNeighbors(int id, int distance);
    int getDividedYearI(Organism *o_1, Organism *o_2);
    int getMinDividedYear(int speciation_year, int group_id_1, int group_id_2, unordered_map<int, vector<Organism*> > *organisms, int current_year_i);
    int distance(int id1, int id2, int limited);
    void markedSpeciesID(int group_id, int temp_species_id, unordered_map<int, vector<Organism*> > *organisms);
    int getTempSpeciesID(int group_id, unordered_map<int, vector<Organism*> > *organisms);
    void generateSpeciationInfo(int year_i);
    virtual ~Simulation();



};

#endif /* DEFINITIONS_SIMULATION_H_ */