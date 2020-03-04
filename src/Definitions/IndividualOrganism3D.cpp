/**
 * @file IndividualOrganism3D.cpp
 * @brief Class IndividualOrganism3D. A class to handle the behavior of an individual in the 3D simulation
 * @author Huijie Qiao
 * @version 1.0
 * @date 3/3/2020
 * @details
 * Copyright 2014-2020 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 */

#include "IndividualOrganism3D.h"

IndividualOrganism3D::IndividualOrganism3D(unsigned p_year,
        SpeciesObject3D* p_species, IndividualOrganism3D* p_parent, unsigned p_id) {
    species = p_species;
    year = p_year;
    parent = p_parent;
    id = p_id;
    groupId = 0;
    tempSpeciesID = 0;
}
void IndividualOrganism3D::setGroupId(unsigned short p_group_id){
    groupId = p_group_id;
}
unsigned short IndividualOrganism3D::getGroupId(){
    return groupId;
}
void IndividualOrganism3D::setTempSpeciesId(unsigned short p_species_id){
    tempSpeciesID = p_species_id;
}
unsigned IndividualOrganism3D::getTempSpeciesId(){
    return tempSpeciesID;
}
void IndividualOrganism3D::setSpecies(SpeciesObject3D* p_species){
    species = p_species;
}
void IndividualOrganism3D::setParent(IndividualOrganism3D* p_parent) {
    parent = p_parent;
}
IndividualOrganism3D* IndividualOrganism3D::getParent() {
    return parent;
}
//void IndividualOrganism3D::removeChild(IndividualOrganism3D* child) {
//    vector<IndividualOrganism3D*>::iterator iter = children.begin();
//    while (iter != children.end()) {
//        if (*iter == child) {
//            iter = children.erase(iter);
//        } else {
//            ++iter;
//        }
//    }
//}
//void IndividualOrganism3D::clearChildren(){
//    for (auto child : children){
//        child->setParent(NULL);
//    }
//}
IndividualOrganism3D::~IndividualOrganism3D() {

}
unsigned IndividualOrganism3D::getNextRunYear() {
    return year + species->getDispersalSpeed();
}
int IndividualOrganism3D::getDispersalMethod() {
    return species->getDispersalMethod();
}
int IndividualOrganism3D::getNumOfPath() {
    return species->getNumOfPath();
}
unsigned short IndividualOrganism3D::getDispersalAbility() {
    return dispersalAbility;
}
void IndividualOrganism3D::setRandomDispersalAbility(){

	double r = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
	unsigned short dispersal_ability = 1;
	for (unsigned short i = 1; i <= species->getDispersalAbilityLength(); i++) {
		if (r <= species->getDispersalAbility()[i-1]) {
			dispersal_ability = i - 1;
			break;
		}
	}
	//LOG(INFO)<<"Dispersal ability is " <<dispersal_ability << " calculated by "<<r;
	dispersalAbility = dispersal_ability;
	//dispersalAbility = 1;
}

void IndividualOrganism3D::setDispersalAbility(unsigned short p_dispersal_ability) {
    dispersalAbility = p_dispersal_ability;
}
unsigned IndividualOrganism3D::getSpeciationYears(){
    return species->getSpeciationYears();
}
//void IndividualOrganism3D::addChild(IndividualOrganism3D* child){
//    children.push_back(child);
//}
bool IndividualOrganism3D::isSuitable(vector<ISEA3H*>* p_current_environments) {
    vector<NicheBreadth*> nicheBreadth = species->getNicheBreadth();
    for (unsigned i = 0; i < nicheBreadth.size(); ++i) {
    	//LOG(INFO)<<"Environments:"<<i<<" Size:"<<(*p_current_environments).size()<<" Address:"<<(*p_current_environments)[i];
        float env_value = (*p_current_environments)[i]->readByID(id);
        //LOG(INFO)<<env_value<<"FALSE";
        if ((env_value > nicheBreadth[i]->getMax())
                || (env_value < nicheBreadth[i]->getMin())) {
            return false;
        }
    }

    //remove the areas covered by the ice sheet randomly based on the ice sheet values.
    if (((*p_current_environments).size()-1)==nicheBreadth.size()){
		int icesheet = (*p_current_environments)[nicheBreadth.size()]->readByID(id);
		double r = static_cast<double>(rand()) * 1000 / static_cast<double>(RAND_MAX);
		//LOG(INFO)<<"icesheet is "<<icesheet<<" and random value is "<<r;
		if (icesheet>=r){
			return false;
		}
    }
    return true;
}
SpeciesObject3D* IndividualOrganism3D::getSpecies() {
    return species;
}
unsigned IndividualOrganism3D::getSpeciesID() {
    return species->getID();
}
unsigned IndividualOrganism3D::getYear(){
    return year;
}
unsigned IndividualOrganism3D::getID(){
    return id;
}

void IndividualOrganism3D::setYear(unsigned p_year){
    year = p_year;
}
unsigned long IndividualOrganism3D::getMemoryUsage(){
	unsigned long mem = 0;
	mem += sizeof(unsigned short) * 4;
	mem += sizeof(unsigned);
	mem += sizeof(SpeciesObject3D*);
	mem += sizeof(vector<IndividualOrganism3D*>);
//	mem += sizeof(IndividualOrganism3D*) * children.size();
	return mem;
}
