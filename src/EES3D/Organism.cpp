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

Organism::Organism(int p_year_i, Species* p_species, Organism* p_parent, int p_id, int p_uid, vector<string> &nb_logs, bool details,
        unordered_map<string, ISEA*> &p_current_environments, ISEA* mask) {
    species = p_species;
    uid = p_uid;
    year_i = p_year_i;
    parent = p_parent;
    id = p_id;
    groupId = 0;
    tempSpeciesID = 0;
    dispersalAbility = 0;
    int parent_uid = 0;
    if (parent){
        evoDirection = p_parent->getEvoDirection();
        nicheBreadthType = setNicheBreadthType(p_species->getNicheBreadthEvolutionRatio(), p_parent->getNicheBreadthType());
        parent_uid = parent->getUid();
    }else{
        for (auto item : p_species->getNicheBreadth()) {
            evoDirection[item.first] = 0;
        }
        nicheBreadthType = setNicheBreadthType(p_species->getNicheBreadthEvolutionRatio(), -1);
    }
    string memo = "";

    for (auto item : p_species->getNicheBreadth()) {
        double env_value = p_current_environments[item.first]->readByID(id);
        envs[item.first] = env_value;
    }
    bool t_details = details;
    switch (nicheBreadthType) {
        case 0:{
            t_details = false;
            //LOG(DEBUG)<<"I DO "<<nicheBreadthType;
            for (auto it : p_species->getNicheBreadth()) {
                NicheBreadth *p_NicheBreadth = it.second;
                NicheBreadth *new_NicheBreadth = new NicheBreadth(p_NicheBreadth->getMin(), p_NicheBreadth->getMax());
                nicheBreadth[it.first] = new_NicheBreadth;
                memo += it.first + "," + to_string(envs[it.first]) + ",";
            }
            break;
        }
        case 1: {
            t_details = false;
            //LOG(DEBUG)<<"I DO "<<nicheBreadthType;
            double r = 1 - 2 * (static_cast<double>(rand()) / static_cast<double>(RAND_MAX));
            r = p_species->getNicheBreadthEvolutionRandomRange() * r;

            //LOG(DEBUG) << "niche breadth ratio is " << r;
            unordered_map<string, NicheBreadth*> niche_breadth = p_species->getNicheBreadth();
            if (parent){
                niche_breadth = parent->getNicheBreadth();
            }
            double rr = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
            if (rr >= p_species->getNicheEnvolutionIndividualRatio()) {
                r = 0;
            }
            memo = to_string(r);
            for (auto it : niche_breadth) {
                NicheBreadth *p_NicheBreadth = it.second;
                double change = (p_NicheBreadth->getMax() - p_NicheBreadth->getMin()) * r;
                NicheBreadth *new_NicheBreadth = new NicheBreadth(p_NicheBreadth->getMin() + change, p_NicheBreadth->getMax() + change);
                nicheBreadth[it.first] = new_NicheBreadth;
                if ((r!=0)&&(details)){
                    memo += it.first + "," + to_string(envs[it.first]) + ",";
                    t_details = true;
                }

            }
            break;
        }
        case 2: {
            //LOG(DEBUG)<<"I DO "<<nicheBreadthType;
            t_details = false;
            unordered_map<string, NicheBreadth*> niche_breadth = p_species->getNicheBreadth();
            if (parent) {
                niche_breadth = parent->getNicheBreadth();
            }
            for (auto it : niche_breadth) {
                NicheBreadth *p_NicheBreadth = it.second;
                NicheBreadth *new_NicheBreadth = new NicheBreadth(p_NicheBreadth->getMin(), p_NicheBreadth->getMax());
                nicheBreadth[it.first] = new_NicheBreadth;
            }
            //if it is unsuitable, skip directly
            if (!isSuitable(mask)){
                break;
            }
            //if it is not a smart species, try to be a smart species
            if (evoDirection[niche_breadth.begin()->first]==0){
                double r = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
                if (r>=p_species->getNicheEnvolutionIndividualRatio()){
                    break;
                }
            }

            for (auto it : niche_breadth) {
                if (evoDirection[it.first]==0){
                    ssamodel s;
                    std::vector<double> X;
                    Organism *temp_org = this->getParent();
                    for (unsigned i = this->species->getNicheBreadthEvolutionParentLevel(); i > 0; i--) {
                        if (!temp_org) {
                            break;
                        }
                        double v = temp_org->getEnv(it.first);
                        if (CommonFun::AlmostEqualRelative(v, (double)NODATA)){
                            break;
                        }
                        X.push_back(v);
                        temp_org = temp_org->getParent();
                    }
                    if (X.size()!=(unsigned)this->species->getNicheBreadthEvolutionParentLevel()){
                        break;
                    }
                    //A group of code of SSA
                    reverse(X.begin(), X.end()); alglib::real_1d_array AX; AX.setcontent(X.size(), &(X[0])); ssacreate(s);
                    int windows = X.size()/2; windows = (windows>5)?5:windows; windows = (windows<3)?3:windows;
                    ssasetwindow(s, windows); ssaaddsequence(s, AX); ssasetalgotopkdirect(s, 2);
                    real_1d_array trend; ssaforecastlast(s, 1, trend);
                    if (details) {
                        memo += "'" + AX.tostring(3) + "'";
                        memo += ",'" + trend.tostring(3) + "'," + to_string(this->envs[it.first]) + "," + to_string(windows) + ",";
                        t_details = true;
                    }
                    double change = trend[0] - this->envs[it.first];
                    evoDirection[it.first] = (change>0)?1:-1;
                    if (it.first=="Debiased_Minimum_Monthly_Temperature"){
                        evoDirection["Debiased_Maximum_Monthly_Temperature"] = evoDirection[it.first];
                    }
                    if (it.first=="Debiased_Maximum_Monthly_Temperature"){
                        evoDirection["Debiased_Minimum_Monthly_Temperature"] = evoDirection[it.first];
                    }
                }
                double r = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
                r = p_species->getNicheBreadthEvolutionRandomRange() * r * evoDirection[it.first];

                double change = (it.second->getMax() - it.second->getMin()) * r;
                if (details) {
                    memo += to_string(r) + "," + to_string(evoDirection[it.first]) + "," + to_string(change);
                }
                NicheBreadth *new_NicheBreadth = new NicheBreadth(it.second->getMin() + change, it.second->getMax() + change);
                delete nicheBreadth[it.first];
                nicheBreadth[it.first] = new_NicheBreadth;
            }
            break;
        }
        case 3: {
            //LOG(DEBUG)<<"I DO "<<nicheBreadthType;
            t_details = false;
            unordered_map<string, NicheBreadth*> niche_breadth = p_species->getNicheBreadth();
            if (parent) {
                niche_breadth = parent->getNicheBreadth();
            }
            for (auto it : niche_breadth) {
                NicheBreadth *p_NicheBreadth = it.second;
                NicheBreadth *new_NicheBreadth = new NicheBreadth(p_NicheBreadth->getMin(), p_NicheBreadth->getMax());
                nicheBreadth[it.first] = new_NicheBreadth;
            }
            //if it is unsuitable, skip directly
            if (!isSuitable(mask)){
                break;
            }
            double r = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
            if (r >= p_species->getNicheEnvolutionIndividualRatio()) {
                break;
            }

            for (auto it : niche_breadth) {
                if (true){
                    ssamodel s;
                    std::vector<double> X;
                    Organism *temp_org = this->getParent();
                    for (unsigned i = this->species->getNicheBreadthEvolutionParentLevel(); i > 0; i--) {
                        if (!temp_org) {
                            break;
                        }
                        double v = temp_org->getEnv(it.first);
                        if (CommonFun::AlmostEqualRelative(v, (double)NODATA)){
                            break;
                        }
                        X.push_back(v);
                        temp_org = temp_org->getParent();
                    }
                    if (X.size()!=(unsigned)this->species->getNicheBreadthEvolutionParentLevel()){
                        break;
                    }
                    //A group of code of SSA
                    reverse(X.begin(), X.end()); alglib::real_1d_array AX; AX.setcontent(X.size(), &(X[0])); ssacreate(s);
                    int windows = X.size()/2; windows = (windows>5)?5:windows; windows = (windows<3)?3:windows;
                    ssasetwindow(s, windows); ssaaddsequence(s, AX); ssasetalgotopkdirect(s, 2);
                    real_1d_array trend; ssaforecastlast(s, 1, trend);
                    if (details) {
                        memo += "'" + AX.tostring(3) + "'";
                        memo += ",'" + trend.tostring(3) + "'," + to_string(this->envs[it.first]) + "," + to_string(windows) + ",";
                        t_details = true;
                    }
                    double change = trend[0] - this->envs[it.first];
                    evoDirection[it.first] = (change>0)?1:-1;
                    if (it.first == "Debiased_Minimum_Monthly_Temperature") {
                        evoDirection["Debiased_Maximum_Monthly_Temperature"] = evoDirection[it.first];
                    }
                    if (it.first == "Debiased_Maximum_Monthly_Temperature") {
                        evoDirection["Debiased_Minimum_Monthly_Temperature"] = evoDirection[it.first];
                    }
                }
                double r = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
                r = p_species->getNicheBreadthEvolutionRandomRange() * r * evoDirection[it.first];

                double change = (it.second->getMax() - it.second->getMin()) * r;
                if (details) {
                    memo += to_string(r) + "," + to_string(evoDirection[it.first]) + "," + to_string(change);
                }
                NicheBreadth *new_NicheBreadth = new NicheBreadth(it.second->getMin() + change, it.second->getMax() + change);
                delete nicheBreadth[it.first];
                nicheBreadth[it.first] = new_NicheBreadth;
            }
            break;
        }
        default: {
            break;
        }
    }

    if (t_details){
        char sql[5000]; // string which will contain the number
        string nb_str = "";
        for (auto it : nicheBreadth){
            char nb[500];
            sprintf(nb, "%s,%f,%f,", it.first.c_str(), it.second->getMin(), it.second->getMax());
            nb_str += nb;
        }

        //sprintf(sql, "INSERT INTO nichebreadth (YEAR, ID, uid, parent_uid, nb_type, nb, memo) VALUES (%d,%d,%d,%d,%d,'%s','%s')",
        //        year_i, id, uid, parent_uid, nicheBreadthType, nb_str.c_str(), memo.c_str());
        sprintf(sql, "%d,%d,%d,%d,%d,%s,%s,%s",
                        year_i, id, uid, parent_uid, nicheBreadthType, species->getIDWithParentID().c_str(), nb_str.c_str(), memo.c_str());
        string sql_c = sql;
        nb_logs.push_back(sql_c);
        //CommonFun::executeSQL(sql_c, log_db, true);
    }

}
unordered_map<string, int> Organism::getEvoDirection(){
    return evoDirection;
}
double Organism::getEnv(string key){
    return envs[key];
}

int Organism::getUid(){
    return uid;
}
int Organism::setNicheBreadthType(vector<double> typeRatio, int parentType){
    vector<double> newRatio = typeRatio;
    int type = parentType;
    //LOG(DEBUG)<<"Parent Type is "<< type;
    //for (unsigned i = 0; i < newRatio.size(); i++) {
    //    LOG(DEBUG) << "old Type " << i << " is  " << newRatio[i];
    //}
    if (parentType!=-1){
        for (unsigned i=newRatio.size()-1; i>0; i--){
            newRatio[i] -= newRatio[i-1];
        }
        newRatio[parentType] = newRatio[parentType] + 1;
        for (unsigned i=0; i<newRatio.size(); i++){
            newRatio[i] /= 2;
        }
    }
    double r = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);

    for (unsigned i = 0; i < newRatio.size(); i++) {
        if (r <= newRatio[i]) {
            type = i;
            break;
        }
    }
    //for (unsigned i=0; i<newRatio.size(); i++){
    //    LOG(DEBUG)<<"new Type " << i << " is  "<< newRatio[i];
    //}

    //LOG(DEBUG)<<"Type is "<< type;
    return type;
}
int Organism::getNicheBreadthType(){
    return nicheBreadthType;
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
    for (auto it : nicheBreadth) {
        delete it.second;
    }
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
unordered_map<string, NicheBreadth*> Organism::getNicheBreadth() {
    return nicheBreadth;
}
//void Organism::addChild(Organism* child){
//    children.push_back(child);
//}
bool Organism::isSuitable(ISEA* mask) {
    for (auto item : nicheBreadth) {
        float mask_value = mask->readByID(id);
        if ((int) mask_value == NODATA) {
            //LOG(DEBUG)<<"NO MASK";
            return false;
        }
        float env_value = envs[item.first];
        if ((int) env_value == NODATA) {
            //LOG(DEBUG)<<"NO DATA";
            return false;
        }

        if ((env_value > item.second->getMax())
                || (env_value < item.second->getMin())) {
            //LOG(DEBUG)<<"UNSUITABLE";
            return false;
        }
    }
    //LOG(DEBUG)<<"SUITABLE";
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
