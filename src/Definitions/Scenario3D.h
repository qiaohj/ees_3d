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
#include "IndividualOrganism3D.h"
#include "../Universal/log.hpp"
#include "Neighbor3D.h"

/**
 * @brief to define the features of a virtual scenario in a simulation, and the virtual species in the scenario.
 */
class Scenario3D {
private:
	/// @brief a sqlite db connection to save the results.
	sqlite3 *log_db;
	/// @brief Whether the simulation runs to the end, or be terminated before the end-time-step because of an exception (such as out of memory, full hard disk, or another exception)
	bool isFinished;
	/// @brief If the target folder exists, overwrite it or skip it.
	bool isOverwrite;
	/// @brief The maximum memory allocated to the application
	unsigned long memLimit;
	/// @brief If save the results to a sqlite database. Suggested to set it to true
	bool isSQLite;
	/// @brief The environmental variables used in the simulation.
	vector<EnvironmentalISEA3H*> environments;
	Neighbor3D* neighborInfo;
	/// @brief The virtual species in the simulation, including the initial species, and new species after the speciation events.
	vector<SpeciesObject3D*> species;

	//useless
	boost::unordered_map<unsigned, boost::unordered_map<unsigned, double>> distances;

	/// @brief The last time step
	unsigned totalYears;

	/// @brief The time span per time step
	unsigned minSpeciesDispersalSpeed;

	/// @brief The mask file of the simulation
	ISEA3H *mask;

	/// @brief The base folder of the simulation
	string baseFolder;

	/// @brief The target folder where the log files is saved.
	string target;
	/**
	 * @brief Remove all the individual objects and release the resources.
	 */
	void cleanActivedIndividualOrganism3Ds();
	/**
	 * @brief Remove all the environmental variable objects and release the resources.
	 */
	void cleanEnvironments();

	/**
	 * @brief Remove all the species object and release the resources.
	 */
	void cleanSpecies();

	/**
	 * @brief Create all the necessary folders for a species
	 * @param p_species the species to create the folder
	 * @param isRoot is a root species of the simulation?
	 */

	void createSpeciesFolder(SpeciesObject3D *p_species, bool isRoot);

	/// @brief Burn-in year of the simulation
	unsigned burnInYear;

	/// @brief A hash map to save all the living individual objects in the simulation.
	boost::unordered_map<unsigned,
			boost::unordered_map<SpeciesObject3D*,
					boost::unordered_map<unsigned, vector<IndividualOrganism3D*> > > > all_individualOrganisms;

	/**
	 * @brief Get the potential distribution of a given individual in the next time step.
	 * @param individualOrganism The given individual object
	 * @param species_folder The folder to save the log file
	 * @param year Time step
	 * @return Return the potential distribution in pixel.
	 */
	set<unsigned>  getDispersalMap_2(IndividualOrganism3D *individualOrganism,
			string species_folder, unsigned year);

	/**
	 * @brief Get the first individual object without a population ID from all the individuals.
	 * @param organisms All the individuals
	 * @return The first individual object
	 */
	IndividualOrganism3D* getUnmarkedOrganism(
			boost::unordered_map<unsigned, vector<IndividualOrganism3D*> > *organisms);

	/**
	 * @brief Allocate a population ID to the individuals which connect with the given individual.
	 * @param p_group_id A group ID to be allocated.
	 * @param p_unmarked_organism The given individual object
	 * @param organisms All the individuals
	 */
	void markJointOrganism(unsigned short p_group_id,
			IndividualOrganism3D *p_unmarked_organism,
			boost::unordered_map<unsigned, vector<IndividualOrganism3D*> > *organisms);

	/**
	 * @brief Get the minimal separating time length of two populations
	 * @param speciation_year Speciation year
	 * @param group_id_1 Population 1
	 * @param group_id_2 Population 2
	 * @param organisms All the individuals
	 * @param current_year Current time step.
	 */
	unsigned getMinDividedYear(unsigned speciation_year,
			unsigned short group_id_1, unsigned short group_id_2,
			boost::unordered_map<unsigned, vector<IndividualOrganism3D*> > *organisms,
			unsigned current_year);

	/**
	 * Get the separating time length of two individuals.
	 * @param o_1 individual 1
	 * @param o_2 individual 2
	 */
	unsigned getDividedYear(IndividualOrganism3D *o_1,
			IndividualOrganism3D *o_2);

	/**
	 * @brief Give a new species ID to all the individuals in a population when a speciation event happens.
	 * @param group_id The population ID
	 * @param temp_species_id species ID
	 * @param organisms All the individuals
	 */
	void markedSpeciesID(unsigned short group_id,
			unsigned short temp_species_id,
			boost::unordered_map<unsigned, vector<IndividualOrganism3D*> > *organisms);

	/**
	 * @brief Get a temporary species ID based on the parent species ID and population ID
	 * @param group_id The population ID
	 * @param organisms All the individuals
	 */
	unsigned short getTempSpeciesID(unsigned short group_id,
			boost::unordered_map<unsigned, vector<IndividualOrganism3D*> > *organisms);
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
	void generateSpeciationInfo(unsigned year, bool is_tree);

	/// @brief Whether outputing the details of the simulation, for debug only.
	bool with_detail;

	/**
	 * @brief Calculate the Distance of two face.
	 * @param id1
	 * @param id2
	 * @return
	 */
	unsigned distance3D(unsigned id1, unsigned id2, unsigned limited);
	/**
	 * @brief return all faces with a given distance.
	 * @param id
	 * @param distance
	 * @return
	 */
	set<unsigned> getNeighbors(unsigned id, unsigned distance);

	/**
	 * @brief create a db used to save the logs
	 * @param path
	 */
	void createDB(const char *path);
public:
	/**
	 * @brief Constructor of Scenario3D class
	 *
	 * @param p_scenario_json_path A path points to the configuration file (in JSON format) which defines the features of the scenario.
	 * @param p_scenario_id A unique ID of the scenario
	 * @param p_base_folder A base folder where the application can load the required resource
	 * @param p_target A folder to save simulation results.
	 * @param p_overwrite Overwrite the folder or not if the folder is existed.
	 * @param p_mem_limit The maximum memory allocated to the application.
	 * @param p_with_detail Output the details of the simulation (all occupy a huge hard disk space) or not. Only for debug.
	 * @param p_isSQLite save the results to a sqlite database.
	 */
	Scenario3D(const string p_scenario_json_path, string p_scenario_id,
			string p_base_folder, string p_target, bool p_overwrite,
			unsigned long p_mem_limit, bool p_with_detail, bool p_isSQLite);

	/**
	 * @brief Destructor of Scenario3D class
	 *
	 * release all the resources
	 */
	virtual ~Scenario3D();

	/**
	 * @brief Run the simulation
	 */
	unsigned run();
	/**
	 * @brief Detect whether the simulation is terminated.
	 * @return Return the terminated reason. TRUE means the simulation is terminated because it finishes the simulation successfully.
	 */
	bool isTerminated();

	/**
	 * @brief Return the environmental layers based on the given time step
	 * @param p_year The time step to get the environmental layers.
	 * @return The environmental layers at the given time step
	 */
	vector<ISEA3H*> getEnvironmenMap(unsigned p_year);

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
	void saveGroupmap(unsigned year,
			boost::unordered_map<SpeciesObject3D*, ISEA3H*> species_group_maps);

	void saveGroupmap_file(unsigned year,
			boost::unordered_map<SpeciesObject3D*, ISEA3H*> species_group_maps);

	void saveGroupmap_db(unsigned year,
			boost::unordered_map<SpeciesObject3D*, ISEA3H*> species_group_maps);
	//bool generateEnv();
};

#endif /* DEFINITIONS_SCENARIO3D_H_ */
