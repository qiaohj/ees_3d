/**
 * @file EnvironmentalISEA3H.h
 * @brief Class EnvironmentalISEA3H. A class to handle the environmental layers in ISEA3H format
 * @author Huijie Qiao
 * @version 1.0
 * @date 3/03/2020
 * @details
 * Copyright 2014-2020 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 */

#ifndef EnvironmentalISEA3H_H_
#define EnvironmentalISEA3H_H_

#include <boost/unordered_map.hpp>
#include "ISEA3H.h"
#include "DBField.h"

/**
 * @brief A class to handle the environmental layers in ISEA3H format
 */
class EnvironmentalISEA3H {
private:
	/**
	 * @brief A hash map to save the environmental layers used in the simulation time step by time step. The keys are the time steps, and the values are the environmental layers on the time step.
	 */
    boost::unordered_map<int, ISEA3H*> layers;

    string envName;
public:

    /**
     * @brief Constructor of EnvironmentalISEA3H class
     * @param p_env_name The table name of the environment in the database.
     * @param p_env_db database
     * @param timeLine timeline the simulation
     */
	EnvironmentalISEA3H(string p_env_name, sqlite3* p_env_db, vector<int> timeLine);

	/**
	 * @brief Destructor of EnvironmentalISEA3H class
	 *
	 * release all the resources
	 */
	virtual ~EnvironmentalISEA3H();

    /**
     * @brief Get the environmental layer of the given time step
     * @param p_year time step
     * @return A ISEA3H object of the environmental layer of the given time step.
     */
    ISEA3H* getValues(int p_year_i);

    /**
     * @brief Get the value based on the id of the face
     * @param p_year time step
     * @param p_id face id
     * @return the value on the id
     */
	float readByID(int p_year_i, int p_id);

};



#endif /* EnvironmentalISEA3H_H_ */
