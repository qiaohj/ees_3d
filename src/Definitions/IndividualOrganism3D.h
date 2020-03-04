/**
 * @file IndividualOrganism3D.h
 * @brief Class IndividualOrganism3D. A class to handle the behavior of an individual in the simulation
 * @author Huijie Qiao
 * @version 1.0
 * @date 3/3/2020
 * @details
 * Copyright 2014-2019 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 */

#ifndef DEFINITIONS_INDIVIDUALORGANISM3D_H_
#define DEFINITIONS_INDIVIDUALORGANISM3D_H_
using namespace std;
#include "SpeciesObject3D.h"
#include "ISEA3H.h"
#include "../Universal/log.hpp"
/**
 * @brief A class to handle the behavior of an individual in the simulation
 */
class IndividualOrganism3D {
private:
    unsigned id;
    unsigned short groupId;
    unsigned short tempSpeciesID;
    unsigned short dispersalAbility;
    SpeciesObject3D* species;
    unsigned year;
    IndividualOrganism3D* parent;
//    vector<IndividualOrganism3D*> children;

public:
    /**
     * @brief Constructor of IndividualOrganism3D class
     * @param p_year the time step of the individual
     * @param p_species the species ID of the individual
     * @param p_parent the parent species ID of the individual
     * @param p_id the id of the face of the individual's locality.
     */
    IndividualOrganism3D(unsigned p_year, SpeciesObject3D* p_species,
            IndividualOrganism3D* p_parent, unsigned p_id);

    /**
	 * @brief Destructor of IndividualOrganism3D class
	 *
	 * release all the resources
	 */
    virtual ~IndividualOrganism3D();

    /**
     * @brief return the next dispersal time step of the individual based on the dispersal speed of the species.
     */
    unsigned getNextRunYear();
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
    unsigned getSpeciationYears();

    /**
     * @brief return the dispersal ability of the individual based on the dispersal ability array of the species.
     */
    unsigned short getDispersalAbility();

    /**
     * @brief set the dispersal ability of the individual based on the dispersal ability array of the species.
     * @param p_dispersal_ability
     */
    void setDispersalAbility(unsigned short p_dispersal_ability);

    /**
     * @brief detect the area where the individual is living is suitable or not, based on the environmental values of the pixel that the individual is in and the niche breadth of the species.
     * @param p_current_environments
     * @return True: suitable False: unsuitable
     */
    bool isSuitable(vector<ISEA3H*>* p_current_environments);

    /**
     * @brief return the species object of the individual
     * @return
     */
    SpeciesObject3D* getSpecies();

    /**
     * @brief return the species ID of the individual
     */
    unsigned getSpeciesID();

    /**
     * @brief set the parent species to the individual
     * @param p_parent
     */
    void setParent(IndividualOrganism3D* p_parent);

    /**
     * @brief set a dispersal ability based on the dispersal ability of the species.
     */
    void setRandomDispersalAbility();

    /**
     * @brief return the parent of the individual
     * @return
     */
    IndividualOrganism3D* getParent();

    /**
     * @brief return the time step which the individual is living
     */
    unsigned getYear();
    /**
	 * @brief return id of faceof the individual
	 */
    unsigned getID();
    /**
     * @brief set the time step which the individual is living.
     * @param p_year
     */
    void setYear(unsigned p_year);
    /**
     * @brief set the population ID of the individual
     * @param p_group_id
     */
    void setGroupId(unsigned short p_group_id);

    /**
	 * @brief return the population ID of the individual
	 */
    unsigned short getGroupId();

    /**
     * @brief set a temporary species ID to the individual
     * @param p_species_id
     */
    void setTempSpeciesId(unsigned short p_species_id);
    /**
     * @brief return the temporary species ID of the individual
     */
    unsigned getTempSpeciesId();

    /**
     * @brief set the species object to the individual
     * @param p_species
     */
    void setSpecies(SpeciesObject3D* p_species);
//    void addChild(IndividualOrganism3D* child);
//    void clearChildren();
//    void removeChild(IndividualOrganism3D* child);

    /**
     * @brief return the memory usage of the object (for debug).
     */
    unsigned long getMemoryUsage();
};

#endif /* DEFINITIONS_INDIVIDUALORGANISM3D_H_ */
