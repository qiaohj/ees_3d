/**
 * @file EnvVar.h
 * @brief Class EnvVar. A class to handle the environmental layers in ISEA format
 * @author Huijie Qiao
 * @version 1.0
 * @date 3/03/2020
 * @details
 * Copyright 2014-2020 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 */

#ifndef EnvVar_H_
#define EnvVar_H_

#include "ISEA.h"
#include "DBField.h"
#include "Utility.h"

/**
 * @brief A class to handle the environmental layers in ISEA format
 */
class EnvVar {
private:
	/**
	 * @brief A hash map to save the environmental layers used in the simulation time step by time step. The keys are the time steps, and the values are the environmental layers on the time step.
	 */
    unordered_map<int, ISEA*> *layers;

    string envName;
public:
    EnvVar();
    /**
     * @brief Constructor of EnvVar class
     * @param p_env_name The table name of the environment in the database.
     * @param p_env_db database
     * @param timeLine timeline the simulation
     */
	EnvVar(string p_env_name, sqlite3* p_env_db, vector<int> *timeLine);

	/**
	 * @brief Destructor of EnvVar class
	 *
	 * release all the resources
	 */
	virtual ~EnvVar();

    /**
     * @brief Get the environmental layer of the given time step
     * @param p_year time step
     * @return A ISEA object of the environmental layer of the given time step.
     */
    ISEA* getValues(int p_year_i);

    /**
     * @brief Get the value based on the id of the face
     * @param p_year time step
     * @param p_id face id
     * @return the value on the id
     */
	float readByID(int p_year_i, int p_id);

};



#endif /* EnvVar_H_ */
