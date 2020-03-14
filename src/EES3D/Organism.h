/**
 * @file Organism.h
 * @brief Class Organism. A class to handle the behavior of an individual in the simulation
 * @author Huijie Qiao
 * @version 1.0
 * @date 3/13/2020
 * @details
 * Copyright 2014-2020 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 */

#ifndef DEFINITIONS_INDIVIDUALORGANISM_H_
#define DEFINITIONS_INDIVIDUALORGANISM_H_

using namespace std;
#include "Species.h"
#include "ISEA.h"
#include "../Universal/easylogging.h"
/**
 * @brief A class to handle the behavior of an individual in the simulation
 */
class Organism {
private:
    int id;
    int groupId;
    int tempSpeciesID;
    int dispersalAbility;
    Species* species;
    int year_i;
    Organism* parent;
//    vector<Organism*> children;

public:
    /**
     * @brief Constructor of Organism class
     * @param p_year the time step of the individual
     * @param p_species the species ID of the individual
     * @param p_parent the parent species ID of the individual
     * @param p_id the id of the face of the individual's locality.
     */
    Organism(int p_year, Species* p_species,
            Organism* p_parent, int p_id);

    /**
	 * @brief Destructor of Organism class
	 *
	 * release all the resources
	 */
    virtual ~Organism();

    /**
     * @brief return the next dispersal time step of the individual based on the dispersal speed of the species.
     */
    int getNextRunYearI();
    /**
     * @brief return the dispersal method (X direction(s) or full directions) based on the dispersal method of the species. (full directions only now)
     * @return
     */
    int getDispersalMethod();

    /**
     * @brief return the number of dispersal direction(s) based on the dispersal method of the species.
     * @return
     */
    int getNumOfPath();

    /**
     * @brief return the speciation years of the species
     */
    int getSpeciationYears();

    /**
     * @brief return the dispersal ability of the individual based on the dispersal ability array of the species.
     */
    int getDispersalAbility();

    /**
     * @brief set the dispersal ability of the individual based on the dispersal ability array of the species.
     * @param p_dispersal_ability
     */
    void setDispersalAbility(int p_dispersal_ability);

    /**
     * @brief detect the area where the individual is living is suitable or not, based on the environmental values of the pixel that the individual is in and the niche breadth of the species.
     * @param p_current_environments
     * @return True: suitable False: unsuitable
     */
    bool isSuitable(unordered_map<string, ISEA*>* p_current_environments, ISEA* mask);

    /**
     * @brief return the species object of the individual
     * @return
     */
    Species* getSpecies();

    /**
     * @brief return the species ID of the individual
     */
    int getSpeciesID();

    /**
     * @brief set the parent species to the individual
     * @param p_parent
     */
    void setParent(Organism* p_parent);

    /**
     * @brief set a dispersal ability based on the dispersal ability of the species.
     */
    void setRandomDispersalAbility();

    /**
     * @brief return the parent of the individual
     * @return
     */
    Organism* getParent();

    /**
     * @brief return the time step which the individual is living
     */
    int getYearI();
    /**
	 * @brief return id of faceof the individual
	 */
    int getID();
    /**
     * @brief set the time step which the individual is living.
     * @param p_year
     */
    void setYearI(int p_year_i);
    /**
     * @brief set the population ID of the individual
     * @param p_group_id
     */
    void setGroupId(int p_group_id);

    /**
	 * @brief return the population ID of the individual
	 */
    int getGroupId();

    /**
     * @brief set a temporary species ID to the individual
     * @param p_species_id
     */
    void setTempSpeciesId(int p_species_id);
    /**
     * @brief return the temporary species ID of the individual
     */
    int getTempSpeciesId();

    /**
     * @brief set the species object to the individual
     * @param p_species
     */
    void setSpecies(Species* p_species);
//    void addChild(Organism* child);
//    void clearChildren();
//    void removeChild(Organism* child);

    /**
     * @brief return the memory usage of the object (for debug).
     */
    int long getMemoryUsage();
};

#endif /* DEFINITIONS_INDIVIDUALORGANISM_H_ */