/**
 * @file Organism.cpp
 * @brief Class Organism. A class to handle the behavior of an individual in the  simulation
 * @author Huijie Qiao
 * @version 1.0
 * @date 3/3/2020
 * @details
 * Copyright 2014-2020 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 */

#include "Organism.h"

Organism::Organism(int p_year_i, Species* p_species, Organism* p_parent, int p_id) {
    species = p_species;
    year_i = p_year_i;
    parent = p_parent;
    id = p_id;
    groupId = 0;
    tempSpeciesID = 0;
    dispersalAbility = 0;
}
void Organism::setGroupId(int p_group_id){
    groupId = p_group_id;
}
int Organism::getGroupId(){
    return groupId;
}
void Organism::setTempSpeciesId(int p_species_id){
    tempSpeciesID = p_species_id;
}
int Organism::getTempSpeciesId(){
    return tempSpeciesID;
}
void Organism::setSpecies(Species* p_species){
    species = p_species;
}
void Organism::setParent(Organism* p_parent) {
    parent = p_parent;
}
Organism* Organism::getParent() {
    return parent;
}

Organism::~Organism() {
}
int Organism::getNextRunYearI() {
    return year_i + species->getDispersalSpeed();
}
int Organism::getDispersalMethod() {
    return species->getDispersalMethod();
}
int Organism::getNumOfPath() {
    return species->getNumOfPath();
}
int Organism::getDispersalAbility() {
    return dispersalAbility;
}
void Organism::setRandomDispersalAbility(){

	double r = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
	int dispersal_ability = 1;
	for (int i = 1; i <= species->getDispersalAbilityLength(); i++) {
		if (r <= species->getDispersalAbility()[i-1]) {
			dispersal_ability = i - 1;
			break;
		}
	}
	//LOG(DEBUG)<<"Dispersal ability is " <<dispersal_ability << " calculated by "<<r;
	dispersalAbility = dispersal_ability;
	//dispersalAbility = 1;
}

void Organism::setDispersalAbility(int p_dispersal_ability) {
    dispersalAbility = p_dispersal_ability;
}
int Organism::getSpeciationYears(){
    return species->getSpeciationYears();
}
//void Organism::addChild(Organism* child){
//    children.push_back(child);
//}
bool Organism::isSuitable(unordered_map<string, ISEA*>* p_current_environments, ISEA* mask) {

    unordered_map<string, NicheBreadth*> *nicheBreadth = species->getNicheBreadth();
    for (auto item : *nicheBreadth) {

        float mask_value = mask->readByID(id);
        if ((int) mask_value == NODATA) {
            LOG(DEBUG)<<"NO MASK";
            return false;
        }
        float env_value = p_current_environments->at(item.first)->readByID(id);
        if ((int) env_value == NODATA) {
            LOG(DEBUG)<<"NO DATA";
            return false;
        }

        if ((env_value > item.second->getMax())
                || (env_value < item.second->getMin())) {
            //LOG(DEBUG)<<"env_value is "<<env_value<<". Max is "<<item.second->getMax()<<" and Min is "<<item.second->getMin();
            return false;
        }
    }

    return true;
}
Species* Organism::getSpecies() {
    return species;
}
int Organism::getSpeciesID() {
    return species->getID();
}
int Organism::getYearI(){
    return year_i;
}
int Organism::getID(){
    return id;
}

void Organism::setYearI(int p_year_i){
    year_i = p_year_i;
}
int long Organism::getMemoryUsage(){
	int long mem = 0;
	mem += sizeof(unsigned short) * 4;
	mem += sizeof(unsigned);
	mem += sizeof(Species*);
	mem += sizeof(vector<Organism*>);
//	mem += sizeof(Organism*) * children.size();
	return mem;
}
