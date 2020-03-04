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
/**
 * @brief A class to handle the environmental layers in ISEA3H format
 */
class EnvironmentalISEA3H {
private:
	/**
	 * @brief A hash map to save the environmental layers used in the simulation time step by time step. The keys are the time steps, and the values are the environmental layers on the time step.
	 */
    boost::unordered_map<unsigned, ISEA3H*> layers;
    /**
     * @brief the burn-in year used in the simulation.
     */
    unsigned burnInYears;

    /**
     * @brief the NODATA value in the environmental layers.
     */
    float noData;

public:

    /**
     * @brief Constructor of EnvironmentalISEA3H class
     * @param p_basefolder The base folder where the application can load the raster files.
     * @param p_burn_in_year Burn-in year of the simulation
     * @param p_begin_year Start time step of the simulation
     * @param p_end_year End time step of the simulation
     * @param p_step The step length of the simulation
     */
	EnvironmentalISEA3H(const std::string p_basefolder, unsigned p_burn_in_year,
			unsigned p_begin_year, unsigned p_end_year, unsigned p_step);

	/**
	 * @brief Destructor of EnvironmentalISEA3H class
	 *
	 * release all the resources
	 */
	virtual ~EnvironmentalISEA3H();

	/**
	 * @brief Return the burn in year.
	 */
    unsigned getBurnInYears();

    /**
     * @brief Get the environmental layer of the given time step
     * @param p_year time step
     * @return A ISEA3H object of the environmental layer of the given time step.
     */
    ISEA3H* getValues(unsigned p_year);

    /**
     * @brief Get the value based on the id of the face
     * @param p_year time step
     * @param p_id face id
     * @return the value on the id
     */
	float readByID(unsigned p_year, unsigned p_id);

};



#endif /* EnvironmentalISEA3H_H_ */
