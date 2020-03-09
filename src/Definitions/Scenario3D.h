/**
 * @file Scenario3D.h
 * @brief Class Scenario3D. A class to define the features of a virtual scenario in a simulation, and the virtual species in the scenario.
 * @author Huijie Qiao
 * @version 1.0
 * @date 3/3/2020
 * @details
 * Copyright 2014-2020 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 */

#ifndef DEFINITIONS_SCENARIO3D_H_
#define DEFINITIONS_SCENARIO3D_H_

using namespace std;
#include <string>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <sqlite3.h>
#include "../Universal/CommonFun.h"
#include "../JsonPaster/include/json/json.h"
#include "../Universal/CommonType.h"
#include "ISEA3H.h"
#include "EnvironmentalISEA3H.h"
#include "SpeciesObject3D.h"
#include "Organism3D.h"
#include "../Universal/easylogging.h"
#include "Neighbor3D.h"
#include "DBField.h"
#include "Simulation3D.h"


/**
 * @brief to define the features of a virtual scenario in a simulation, and the virtual species in the scenario.
 */
class Scenario3D {
private:
	/// @brief a sqlite db connection to save the results.
	unsigned long memLimit;
	vector<int> timeLine;
	Neighbor3D* neighborInfo;
	/// @brief If save the results to a sqlite database. Suggested to set it to true
	/// @brief The environmental variables used in the simulation.
	boost::unordered_map<string, EnvironmentalISEA3H*> environments_base;
	//boost::unordered_map<string, EnvironmentalISEA3H*> environments;

	/// @brief The virtual species in the simulation, including the initial species, and new species after the speciation events.
	vector<Simulation3D*> initSimulations(sqlite3* conf_db, sqlite3* env_db, int p_id, string p_target, bool p_overwrite, Neighbor3D* neighborInfo);

	/**
	 * @brief Remove all the individual objects and release the resources.
	 */
	void cleanActivedOrganism3Ds();
	/**
	 * @brief Remove all the environmental variable objects and release the resources.
	 */
	void cleanEnvironments();

	/**
	 * @brief Remove all the species object and release the resources.
	 */
	void cleanSpecies();


	/// @brief Burn-in year of the simulation
	int burnInYear;



	/**
	 * @brief Get the potential distribution of a given individual in the next time step.
	 * @param individualOrganism The given individual object
	 * @param species_folder The folder to save the log file
	 * @param year Time step
	 * @return Return the potential distribution in pixel.
	 */
	set<int>  getDispersalMap_2(Organism3D *individualOrganism,
			string species_folder, int year);

	/**
	 * @brief Get the first individual object without a population ID from all the individuals.
	 * @param organisms All the individuals
	 * @return The first individual object
	 */
	Organism3D* getUnmarkedOrganism(
			boost::unordered_map<int, vector<Organism3D*> > *organisms);

	/**
	 * @brief Allocate a population ID to the individuals which connect with the given individual.
	 * @param p_group_id A group ID to be allocated.
	 * @param p_unmarked_organism The given individual object
	 * @param organisms All the individuals
	 */
	void markJointOrganism(int short p_group_id,
			Organism3D *p_unmarked_organism,
			boost::unordered_map<int, vector<Organism3D*> > *organisms);

	/**
	 * @brief Get the minimal separating time length of two populations
	 * @param speciation_year Speciation year
	 * @param group_id_1 Population 1
	 * @param group_id_2 Population 2
	 * @param organisms All the individuals
	 * @param current_year Current time step.
	 */
	int getMinDividedYear(int speciation_year,
			int short group_id_1, int short group_id_2,
			boost::unordered_map<int, vector<Organism3D*> > *organisms,
			int current_year);

	/**
	 * Get the separating time length of two individuals.
	 * @param o_1 individual 1
	 * @param o_2 individual 2
	 */
	int getDividedYear(Organism3D *o_1,
			Organism3D *o_2);

	/**
	 * @brief Give a new species ID to all the individuals in a population when a speciation event happens.
	 * @param group_id The population ID
	 * @param temp_species_id species ID
	 * @param organisms All the individuals
	 */
	void markedSpeciesID(int short group_id,
			int short temp_species_id,
			boost::unordered_map<int, vector<Organism3D*> > *organisms);

	/**
	 * @brief Get a temporary species ID based on the parent species ID and population ID
	 * @param group_id The population ID
	 * @param organisms All the individuals
	 */
	int short getTempSpeciesID(int short group_id,
			boost::unordered_map<int, vector<Organism3D*> > *organisms);
	/**
	 * @brief Get the folder of the given species
	 * @param p_species The species to return the folder
	 * @return The folder of the species in the parameter.
	 */
	string getSpeciesFolder(SpeciesObject3D *p_species);
	/**
	 * @brief Generate the speciation information of the simulation, including the speciation tree (in both NEXUS and HTML format), number of speciation and extinction, etc.
	 * @param year time step
	 * @param is_tree Whether generating the speciation tree.
	 */
	void generateSpeciationInfo(int year, bool is_tree);

	/// @brief Whether outputing the details of the simulation, for debug only.
	bool with_detail;

	/**
	 * @brief Calculate the Distance of two face.
	 * @param id1
	 * @param id2
	 * @return
	 */
	int distance3D(int id1, int id2, int limited);
	/**
	 * @brief return all faces with a given distance.
	 * @param id
	 * @param distance
	 * @return
	 */
	set<int> getNeighbors(int id, int distance);

	/**
	 * @brief create a db used to save the logs
	 * @param path
	 */
	void createDB(const char *path);

	/**
     * @brief Run the simulation
     */
    int run();

public:
	/**
	 * @brief Constructor of Scenario3D class
	 *
	 * @param p_env_db A path points to the environment database.
	 * @param p_conf_db A path points to the configuration database
	 * @param p_id ID of the simualtion in the p_conf_db. -1 means all simulations in it.
	 * @param p_target A folder to save simulation results.
	 * @param p_overwrite Overwrite the folder or not if the folder is existed.
	 * @param p_mem_limit The maximum memory allocated to the application.
	 */
	Scenario3D(string p_env_db, string p_conf_db, string p_target, bool p_overwrite, int p_id, unsigned long p_mem_limit);

	/**
	 * @brief Destructor of Scenario3D class
	 *
	 * release all the resources
	 */
	virtual ~Scenario3D();


	/**
	 * @brief Detect whether the simulation is terminated.
	 * @return Return the terminated reason. TRUE means the simulation is terminated because it finishes the simulation successfully.
	 */
	bool isTerminated();
	void initEnvironments(sqlite3* env_db);
	sqlite3* openDB(string p_db);
	/**
	 * @brief Return the environmental layers based on the given time step
	 * @param p_year The time step to get the environmental layers.
	 * @return The environmental layers at the given time step
	 */
	boost::unordered_map<string, ISEA3H*> getEnvironmenMap(int p_year);

	/**
	 * @brief Return the folder to save the result of the simulation.
	 * @return The folder to save the result of the simulation.
	 */
	string getTarget();

	/**
	 * @brief Save the distribution map, the population, and the speciation information to the log file
	 * @param year The time step to save the result
	 * @param species_group_maps The population information to save
	 */
	void saveGroupmap(int year,
			boost::unordered_map<SpeciesObject3D*, ISEA3H*> species_group_maps);

	//bool generateEnv();
};

#endif /* DEFINITIONS_SCENARIO3D_H_ */
