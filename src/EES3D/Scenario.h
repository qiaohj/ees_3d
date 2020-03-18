/**
 * @file Scenario.h
 * @brief Class Scenario. A class to define the features of a virtual scenario in a simulation, and the virtual species in the scenario.
 * @author Huijie Qiao
 * @version 1.0
 * @date 3/3/2020
 * @details
 * Copyright 2014-2020 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 */

#ifndef DEFINITIONS_SCENARIO_H_
#define DEFINITIONS_SCENARIO_H_

using namespace std;
#include <string>
#include <sqlite3.h>
#include "../Universal/easylogging.h"
#include "../Universal/CommonFun.h"
#include "Neighbor.h"
#include "DBField.h"
#include "Simulation.h"
#include "ISEA.h"
#include "EnvVar.h"
#include "Species.h"
#include "Organism.h"


/**
 * @brief to define the features of a virtual scenario in a simulation, and the virtual species in the scenario.
 */
class Scenario {
private:
	/// @brief a sqlite db connection to save the results.
	unsigned long memLimit;
	vector<int> *timeLine;
	Neighbor* neighborInfo;
	unordered_map<string, ISEA*> *masks;
	/// @brief If save the results to a sqlite database. Suggested to set it to true
	/// @brief The environmental variables used in the simulation.
	unordered_map<string, EnvVar*> *environments_base;
	/// @brief The virtual species in the simulation, including the initial species, and new species after the speciation events.
	void initSimulations(sqlite3* conf_db, sqlite3* env_db, int p_id, string p_target, bool p_overwrite, Neighbor* neighborInfo, vector<Simulation*> *simulations);
	/// @brief Burn-in year of the simulation
	int burnInYear;
	/// @brief Whether outputing the details of the simulation, for debug only.
	bool with_detail;
	/**
     * @brief Run the simulation
     */
    int run();

public:
	/**
	 * @brief Constructor of Scenario class
	 *
	 * @param p_env_db A path points to the environment database.
	 * @param p_conf_db A path points to the configuration database
	 * @param p_id ID of the simualtion in the p_conf_db. -1 means all simulations in it.
	 * @param p_target A folder to save simulation results.
	 * @param p_overwrite Overwrite the folder or not if the folder is existed.
	 * @param p_mem_limit The maximum memory allocated to the application.
	 */
	Scenario(string p_env_db, string p_conf_db, string p_target, bool p_overwrite, int p_id, unsigned long p_mem_limit);

	/**
	 * @brief Destructor of Scenario class
	 *
	 * release all the resources
	 */
	virtual ~Scenario();


	/**
	 * @brief Detect whether the simulation is terminated.
	 * @return Return the terminated reason. TRUE means the simulation is terminated because it finishes the simulation successfully.
	 */
	bool isTerminated();
	void initEnvironments(sqlite3* env_db);
	sqlite3* openDB(string p_db);
};

#endif /* DEFINITIONS_SCENARIO_H_ */
