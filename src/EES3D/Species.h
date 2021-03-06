/**
 * @file Species.h
 * @brief Class Species. A class to handle the attributes and behaviors of a virtual species
 * @author Huijie Qiao
 * @version 1.0
 * @date 3/3/2020
 * @details
 * Copyright 2014-2020 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 */


#ifndef DEFINITIONS_Species_H_
#define DEFINITIONS_Species_H_
using namespace std;
#include <string>
#include <vector>
#include <sqlite3.h>
#include <set>
#include "../Universal/easylogging.h"
#include "../Universal/CommonFun.h"
#include "DBField.h"
#include "NicheBreadth.h"


/**
 * @brief A class to handle the attributes and behaviors of a virtual species
 */
class Species {
private:
    bool newSpecies;
    int global_id;
    unordered_map<string, NicheBreadth*> nicheBreadth;
    double* dispersalAbility;
    int dispersalAbilityLength;
    int dispersalSpeed;
    int speciationYears;
    int dispersalMethod;
    int numberOfPath;
    int appearedYearI;
    int disappearedYearI;
    int burninYear;
    unsigned speciesExtinctionThreshold;
    int groupExtinctionThreshold;
    int speciesExtinctionTimeSteps;
    int currentSpeciesExtinctionTimeSteps;
    vector<double> nicheBreadthEvolutionRatio;
    double nicheBreadthEvolutionRandomRange;
    int nicheBreadthEvolutionParentLevel;
    double nicheEnvolutionIndividualRatio;
    double speciesExtinctionThreaholdPercentage;
    unsigned maxSpeciesDistribution;
    set<int> seeds;
    Species* parent;
    vector<Species*> children;
    int number_of_clade_extinction;
    int number_of_speciation;
    int number_of_species_extinction;
    /// @brief 0: unknown, 1: extincted, 2: unextincted 3: parent_extincted
    int clade_extinction_status;
    vector<string> environment_labels;
    vector<int> timeLine;
    int from;
    int to;
    int step;
public:
    /**
     * @brief Constructor of Species class No.1 (Create a species object based on the configuration in JSON format)
     * @param json_path the path to configuration file in JSON format
     */
    Species(sqlite3_stmt *stmt, int burn_in_year, vector<int> p_timeLine,
    		int p_from, int p_to, int p_step);
    /**
     * @brief Constructor of Species class No.2 (Create a species object based on the parent species)
     * @param p_id species ID
     * @param p_parent the parent species object
     * @param p_year time step
     */
    Species(int p_id, Species* p_parent, int p_year_i);
    /**
	 * @brief Destructor of Species class
	 *
	 * release all the resources
	 */
    virtual ~Species();
    /**
     * @brief return the dispersal ability array
     * @return
     */
    double* getDispersalAbility();
    vector<int> getTimeLine();
    int getBurnInYear();

    /**
	 * @brief return the dispersal speed
	 * @return
	 */
    int getDispersalSpeed();
    /**
	 * @brief return the length of the dispersal ability array
	 * @return
	 */
    int getDispersalAbilityLength();
    vector<double> getDispersalAbilityProb();
    /**
	 * @brief return the species extinction threshold. If the area of the distribution (number of the pixels occupied by the species) is smaller than the threshold for some time steps (defined with SpeciesExtinctionTimeSteps), the species will go extinct.
	 * @return
	 */
    unsigned getSpeciesExtinctionThreshold();
    /**
	 * @brief return the population extinction threshold. If the area of the distribution (number of the pixels occupied by a population) is smaller than the threshold, the population will go extinct.
	 * @return
	 */
    int getGroupExtinctionThreshold();

    vector<double> getNicheBreadthEvolutionRatio();
    int getNicheBreadthEvolutionParentLevel();
    double getNicheEnvolutionIndividualRatio();
    double getNicheBreadthEvolutionRandomRange();
    /**
	 * @brief The maximum time steps for a species to go extinct when its distribution is smaller than a threshold defined with SpeciesExtinctionThreshold or SpeciesExtinctionThreaholdPercentage
	 * @return
	 */
    int getSpeciesExtinctionTimeSteps();
    /**
     * @brief get the length of time (number of time steps or duration) of a species which its distribution is smaller than the threshold defined with SpeciesExtinctionThreshold or SpeciesExtinctionThreaholdPercentage
     */
    int getCurrentSpeciesExtinctionTimeSteps();
    /**
     * @brief get the maximum distribution (number of the pixels) of the species during the past simulation. This parameter is used to calculate the ratio of the current distribution with the maximum distribution. If the ratio is smaller than a threshold (SpeciesExtinctionThreaholdPercentage), it will be counted as an extinction.
     */
    unsigned getMaxSpeciesDistribution();
    /**
     * @brief get the maximum distribution (number of the pixels) of the species
     * @param distribution
     */
    void setMaxSpeciesDistribution(unsigned p_max_species_distribution);
    /**
     * @brief get the 'ratio' extinction threshold. The ratio if the current distribution/maximum distribution.
     * @return
     */
    double getSpeciesExtinctionThreaholdPercentage();
    /**
     * @brief A counter to save the current extinction time steps. If the distribution of the species touches the threshold of extinction, invoke this function to add the counter.
     */
    void addCurrentSpeciesExtinctionTimeSteps();
    /**
     * @brief set the counter of the current extinction time steps.
     * @param p_currentSpeciesExtinctionTimeSteps
     */
    void setCurrentSpeciesExtinctionTimeSteps(int p_currentSpeciesExtinctionTimeSteps);
    /**
     * @brief return the dispersal method. 1: move to X directions. X is defined with NumOfPath. 2: move to all the potential directions.
     * @return
     */
    int getDispersalMethod();
    /**
     * @brief get the seed of the species.
     * @return
     */
    set<int> getSeeds();
    /**
	 * @brief get the species ID
	 * @return
	 */
    int getID();
    /**
	 * @brief get the potential directions for an individual to move.
	 * @return
	 */
    int getNumOfPath();
    /**
     * @brief return the niche breadths of the species
     * @return
     */
    unordered_map<string, NicheBreadth*> getNicheBreadth();
    /**
     * @brief return the speciation year
     */
    int getSpeciationYears();
    /**
     * @brief return the parent species of current species
     * @return
     */
    Species* getParent();
    /**
     * @brief set the time step when the species goes extinct.
     * @param p_disappeared_year
     */
    void setDisappearedYearI(int p_disappeared_year_i);
    /**
     * @brief get the time step when the species goes extinct.
     */
    int getDisappearedYearI();
    /**
	 * @brief get the time step that the species appears.
	 */
    int getAppearedYearI();
    /**
     * @brief get all the children species
     * @return
     */
    vector<Species*> getChildren();
    /**
     * @brief add a child species.
     * @param child
     */
    void addChild(Species* child);
    /**
     * @brief get the tree of the species with its children species
     * @param isroot whether this species is a root species (the first species in the simulation)
     * @param iscolor generating a colorful tree or not.
     * @param p_year the first time step to generate the tree
     * @return return a speciation tree in NEXUS format.
     */
    string getNewickTree(bool isroot, bool iscolor);
    /**
     * @brief count the number of speciation/species extinction/clade extinction in this species and its children.
     * @param total_years
     */
    void markNode();
    /**
     * @brief get the clade extinction status of this species. 0: not extinct 1: extinct
     * @param status 0: unknown, 1: extincted, 2: unextincted 3: parent_extincted
     */
    void setCladeExtinctionStatus(int status);
    /**
     * @brief set the parent clade status. 0: unknown, 1: extincted, 2: unextincted 3: parent_extincted
     */
    void markParentClade();
    /**
     * @brief detect all the extinct status of all the children of this species.
     * @param total_years if the last appear year of the species is total_years, it means the species doesn't go extinct, or it goes extinct.
     * @return
     */
    bool isAllLeafExtinction();
    /**
     * @brief return the number of clade extinction
     */
    int getNumberOfCladeExtinction();
    /**
	 * @brief return the number of speciation
	 */
    int getNumberOfSpeciation();
    /**
	 * @brief return the number of species extinction
	 */
    int getNumberOfSpeciesExtinction();
    /**
     * @brief get the tree of the species with its children species in HTML format
     * @param p_year
     * @return
     */
    vector<string> getHTMLTree();
    /**
     * @brief get the string with CSV format which saves the number of speciation/clade extinction/species extinction.
     * @param isroot
     * @param total_years
     * @return
     */
    string getSpeciationExtinction(bool isroot);
    /**
     * @brief is a new species or the raw species from the configuration (JSON format)
     * @return TRUE or FALSE
     */
    bool isNewSpecies();
    /**
     * @brief set the species status.
     * @param p TRUE or FALSE
     */
    void setNewSpecies(bool p);
    /**
     * @brief get the species ID with its parent ID
     * @return
     */
    string getIDWithParentID();
    vector<string> getEnvironmentLabels();
    int getEvoType();
    int getFrom();
    int getTo();
    int getStep();

};

#endif /* DEFINITIONS_SPECIESOBJECT_H_ */
