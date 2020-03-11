/**
 * @file Organism3D.cpp
 * @brief Class Organism3D. A class to handle the behavior of an individual in the 3D simulation
 * @author Huijie Qiao
 * @version 1.0
 * @date 3/3/2020
 * @details
 * Copyright 2014-2020 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 */

#include "Organism3D.h"

Organism3D::Organism3D(int p_year_i, SpeciesObject3D* p_species, Organism3D* p_parent, int p_id) {
    species = p_species;
    year_i = p_year_i;
    parent = p_parent;
    id = p_id;
    groupId = 0;
    tempSpeciesID = 0;
    dispersalAbility = 0;
}
void Organism3D::setGroupId(int p_group_id){
    groupId = p_group_id;
}
int Organism3D::getGroupId(){
    return groupId;
}
void Organism3D::setTempSpeciesId(int p_species_id){
    tempSpeciesID = p_species_id;
}
int Organism3D::getTempSpeciesId(){
    return tempSpeciesID;
}
void Organism3D::setSpecies(SpeciesObject3D* p_species){
    species = p_species;
}
void Organism3D::setParent(Organism3D* p_parent) {
    parent = p_parent;
}
Organism3D* Organism3D::getParent() {
    return parent;
}
//void Organism3D::removeChild(Organism3D* child) {
//    vector<Organism3D*>::iterator iter = children.begin();
//    while (iter != children.end()) {
//        if (*iter == child) {
//            iter = children.erase(iter);
//        } else {
//            ++iter;
//        }
//    }
//}
//void Organism3D::clearChildren(){
//    for (auto child : children){
//        child->setParent(NULL);
//    }
//}
Organism3D::~Organism3D() {
    //delete species;
}
int Organism3D::getNextRunYearI() {
    return year_i + species->getDispersalSpeed();
}
int Organism3D::getDispersalMethod() {
    return species->getDispersalMethod();
}
int Organism3D::getNumOfPath() {
    return species->getNumOfPath();
}
int Organism3D::getDispersalAbility() {
    return dispersalAbility;
}
void Organism3D::setRandomDispersalAbility(){

	double r = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
	int dispersal_ability = 1;
	for (int i = 1; i <= species->getDispersalAbilityLength(); i++) {
		if (r <= species->getDispersalAbility()[i-1]) {
			dispersal_ability = i - 1;
			break;
		}
	}
	//LOG(INFO)<<"Dispersal ability is " <<dispersal_ability << " calculated by "<<r;
	dispersalAbility = dispersal_ability;
	//dispersalAbility = 1;
}

void Organism3D::setDispersalAbility(int p_dispersal_ability) {
    dispersalAbility = p_dispersal_ability;
}
int Organism3D::getSpeciationYears(){
    return species->getSpeciationYears();
}
//void Organism3D::addChild(Organism3D* child){
//    children.push_back(child);
//}
bool Organism3D::isSuitable(boost::unordered_map<string, ISEA3H*>* p_current_environments, ISEA3H* mask) {
    boost::unordered_map<string, NicheBreadth*> nicheBreadth = species->getNicheBreadth();
    for (auto item : nicheBreadth) {
    	//LOG(INFO)<<"Environments:"<<i<<" Size:"<<(*p_current_environments).size()<<" Address:"<<(*p_current_environments)[i];
        float mask_value = mask->readByID(id);
        if ((int) mask_value == NODATA) {
            return false;
        }
        float env_value = (*p_current_environments)[item.first]->readByID(id);
        if ((int) env_value == NODATA) {
            return false;
        }

        //LOG(INFO)<<env_value<<"FALSE";
        if ((env_value > item.second->getMax())
                || (env_value < item.second->getMin())) {
            return false;
        }
    }

    return true;
}
SpeciesObject3D* Organism3D::getSpecies() {
    return species;
}
int Organism3D::getSpeciesID() {
    return species->getID();
}
int Organism3D::getYearI(){
    return year_i;
}
int Organism3D::getID(){
    return id;
}

void Organism3D::setYearI(int p_year_i){
    year_i = p_year_i;
}
int long Organism3D::getMemoryUsage(){
	int long mem = 0;
	mem += sizeof(unsigned short) * 4;
	mem += sizeof(unsigned);
	mem += sizeof(SpeciesObject3D*);
	mem += sizeof(vector<Organism3D*>);
//	mem += sizeof(Organism3D*) * children.size();
	return mem;
}
