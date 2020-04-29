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
        unordered_map<string, ISEA*> &p_current_environments, ISEA* mask, int p_evoType) {
    species = p_species;
    uid = p_uid;
    year_i = p_year_i;
    parent = p_parent;
    id = p_id;
    groupId = 0;
    tempSpeciesID = 0;
    dispersalAbility = 0;
    int parent_uid = 0;
    evoType = p_evoType;
    if (parent){
        evoDirection = p_parent->getEvoDirection();
        //nicheBreadthType = setNicheBreadthType(p_species->getNicheBreadthEvolutionRatio(), p_parent->getNicheBreadthType());
        parent_uid = parent->getUid();
    }else{
        for (auto item : p_species->getNicheBreadth()) {
            evoDirection[item.first] = 0;
        }
        //nicheBreadthType = setNicheBreadthType(p_species->getNicheBreadthEvolutionRatio(), -1);
    }
    string memo = "";

    for (auto item : p_species->getNicheBreadth()) {
        double env_value = p_current_environments[item.first]->readByID(id);
        envs[item.first] = env_value;
    }
    int evoType_temp = evoType;
    if ((year_i<=10) && (evoType == 5)) {
        evoType_temp = 2;
        for (string env_label : species->getEnvironmentLabels()){
            this->nicheBreadthEvolutionRatio[env_label] = species->getNicheBreadthEvolutionRatio();
        }
        details = false;
    }
    if (evoType==6){
        evoType_temp = 2;
    }
    bool t_details = details;
    //LOG(INFO)<<"evoType_temp is "<<evoType_temp;
    switch (evoType_temp) {
        case 1:{
            t_details = false;
            //LOG(DEBUG)<<"1. I DO "<<nicheBreadthType;
            for (auto it : p_species->getNicheBreadth()) {
                NicheBreadth *p_NicheBreadth = it.second;
                NicheBreadth *new_NicheBreadth = new NicheBreadth(p_NicheBreadth->getMin(), p_NicheBreadth->getMax());
                nicheBreadth[it.first] = new_NicheBreadth;
                //memo += it.first + "," + to_string(envs[it.first]) + ",";
            }
            break;
        }
        case 2: {
            t_details = false;
            //LOG(DEBUG)<<"2. I DO "<<nicheBreadthType;
            double r = 0;
            double rr = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
            if (rr < p_species->getNicheEnvolutionIndividualRatio()) {
                r = 1 - 2 * (static_cast<double>(rand()) / static_cast<double>(RAND_MAX));
                r = p_species->getNicheBreadthEvolutionRandomRange() * r;
            }
            //LOG(DEBUG) << "niche breadth ratio is " << r;
            unordered_map<string, NicheBreadth*> niche_breadth = p_species->getNicheBreadth();
            if (parent){
                niche_breadth = parent->getNicheBreadth();
            }

            memo = to_string(r) + ",";
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
        case 3: {
            //LOG(INFO)<<"3. I DO ";
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
            if (isSuitable(mask)!=1){
                break;
            }
            //if it is not a smart species, try to be a smart species
            if (evoDirection[niche_breadth.begin()->first]==0){
                double r = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
                //LOG(INFO)<<"2 "<<p_species->getNicheEnvolutionIndividualRatio() << " : "<< r;
                if (r>=p_species->getNicheEnvolutionIndividualRatio()){
                    break;
                }
            }
            for (auto it : niche_breadth) {
                if (evoDirection[it.first]==0){
                    ssamodel s;
                    std::vector<double> X;
                    Organism *temp_org = this->getParent();
                    for (int i = this->species->getNicheBreadthEvolutionParentLevel(); i >= 0; i--) {
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
                    if (X.size()<(unsigned)this->species->getNicheBreadthEvolutionParentLevel()){
                        break;
                    }
                    //A group of code of SSA
                    reverse(X.begin(), X.end()); alglib::real_1d_array AX; AX.setcontent(X.size(), &(X[0])); ssacreate(s);
                    int windows = X.size(); windows = (windows>5)?5:windows; windows = (windows<3)?3:windows;
                    ssasetwindow(s, windows); ssaaddsequence(s, AX); ssasetalgotopkdirect(s, 2);
                    real_1d_array trend; ssaforecastlast(s, 1, trend);
                    if (details) {
                        memo += it.first + ",'" + AX.tostring(3) + "',";
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
                    memo += to_string(r) + "," + to_string(evoDirection[it.first]) + "," + to_string(change) + ",";
                    t_details = true;
                }
                NicheBreadth *new_NicheBreadth = new NicheBreadth(it.second->getMin() + change, it.second->getMax() + change);
                delete nicheBreadth[it.first];
                nicheBreadth[it.first] = new_NicheBreadth;
            }
            break;
        }
        case 4: {
            //LOG(DEBUG)<<"4. I DO "<<nicheBreadthType;
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
        case 5: {
            this->nicheBreadthEvolutionRatio = parent->getNicheBreadthEvolutionRatio();
            //LOG(DEBUG)<<"5. I DO "<<nicheBreadthType;
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
            double rr = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
            if (rr >= p_species->getNicheEnvolutionIndividualRatio()) {
                break;
            }
            //Debiased_Minimum_Monthly_Temperature, Debiased_Maximum_Monthly_Temperature, Debiased_Maximum_Monthly_Precipitation

            if ((rr != 0) && (details)) {
                for (auto it : this->nicheBreadthEvolutionRatio) {
                    memo += it.first + ",";
                    for (auto c_it : it.second) {
                        memo += to_string(c_it) + ",";
                    }
                }

                t_details = true;
            }
            vector<string> env_keys;
            env_keys.push_back("Debiased_Maximum_Monthly_Temperature");
            env_keys.push_back("Debiased_Maximum_Monthly_Precipitation");
            for (auto c_it : env_keys) {

                rr = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
                int evo_type = 0;
                for (auto p_it : nicheBreadthEvolutionRatio[c_it]) {
                    if (rr <= p_it) {
                        break;
                    }
                    evo_type++;
                }
                rr = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
                rr = p_species->getNicheBreadthEvolutionRandomRange() * rr;
                double change_ratio = 0.5;
                vector<double> ratioProb = this->getNicheBreadthEvolutionRatioProb(c_it);
                if (evo_type == 1) {
                    rr = rr * -1;
                    change_ratio *= ratioProb[1];
                    //change_ratio = (change_ratio>ratioProb[0])?(ratioProb[0] - 0.01):change_ratio;
                    change_ratio = (change_ratio > ratioProb[2]) ? (ratioProb[2] - 0.01) : change_ratio;

                    //ratioProb[0] -= change_ratio;
                    ratioProb[1] += change_ratio;
                    ratioProb[2] -= change_ratio;
                } else {
                    change_ratio *= ratioProb[2];
                    //change_ratio = (change_ratio>ratioProb[0])?(ratioProb[0] - 0.01):change_ratio;
                    change_ratio = (change_ratio > ratioProb[1]) ? (ratioProb[1] - 0.01) : change_ratio;

                    //ratioProb[0] -= change_ratio;
                    ratioProb[1] -= change_ratio;
                    ratioProb[2] += change_ratio;
                }
                this->setNicheBreadthEvolutionRatio(ratioProb, c_it);

                NicheBreadth *p_NicheBreadth = niche_breadth[c_it];
                double change = (p_NicheBreadth->getMax() - p_NicheBreadth->getMin()) * rr;
                NicheBreadth *new_NicheBreadth = new NicheBreadth(p_NicheBreadth->getMin() + change, p_NicheBreadth->getMax() + change);
                delete nicheBreadth[c_it];
                nicheBreadth[c_it] = new_NicheBreadth;
                if (c_it == "Debiased_Maximum_Monthly_Temperature") {
                    this->setNicheBreadthEvolutionRatio(ratioProb, "Debiased_Minimum_Monthly_Temperature");
                    NicheBreadth *p_NicheBreadth = niche_breadth["Debiased_Minimum_Monthly_Temperature"];
                    NicheBreadth *new_NicheBreadth = new NicheBreadth(p_NicheBreadth->getMin() + change, p_NicheBreadth->getMax() + change);
                    delete nicheBreadth["Debiased_Minimum_Monthly_Temperature"];
                    nicheBreadth["Debiased_Minimum_Monthly_Temperature"] = new_NicheBreadth;
                }

            }
            for (auto it : niche_breadth) {
                if (details) {
                    memo += it.first + "," + to_string(envs[it.first]) + ",";
                    t_details = true;
                }
            }
            break;
        }
        default: {
            //LOG(DEBUG)<<"Default. I DO "<<nicheBreadthType;
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
                        year_i, id, uid, parent_uid, evoType, species->getIDWithParentID().c_str(), nb_str.c_str(), memo.c_str());
        string sql_c = sql;
        nb_logs.push_back(sql_c);
        //CommonFun::executeSQL(sql_c, log_db, true);
    }

}
void Organism::setNicheBreadthEvolutionRatio(vector<double> ratios, string env_key){
    for (unsigned i = 0; i < ratios.size(); i++) {
        if (i == 0) {
            nicheBreadthEvolutionRatio[env_key][i] = ratios[i];
        } else {
            nicheBreadthEvolutionRatio[env_key][i] = nicheBreadthEvolutionRatio[env_key][i - 1] + ratios[i];
        }
    }
}
unordered_map<string, vector<double>> Organism::getNicheBreadthEvolutionRatio(){
    return nicheBreadthEvolutionRatio;
}
vector<double> Organism::getNicheBreadthEvolutionRatioProb(string env_key){
    vector<double> ratios;
    for (unsigned i=0; i<nicheBreadthEvolutionRatio[env_key].size(); i++){
        if (i==0){
            ratios.push_back(nicheBreadthEvolutionRatio[env_key][i]);
        }else{
            ratios.push_back(nicheBreadthEvolutionRatio[env_key][i] - nicheBreadthEvolutionRatio[env_key][i-1]);
        }
    }
    return ratios;
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
int Organism::setNicheBreadthType_removed(vector<double> typeRatio, int parentType){
    if (parentType==4){
        return parentType;
    }
    for (double v : typeRatio){
        if ((!CommonFun::AlmostEqualRelative(v, 1.0))&&(!CommonFun::AlmostEqualRelative(v, 0.0))){
            return 4;
        }
    }
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
/**
 * -1 mean unsuitable because of map 0: mean unsuitable because of environment, 1: mean suitable
 * @param mask
 * @return
 */
int Organism::isSuitable(ISEA* mask) {
    for (auto item : nicheBreadth) {
        float mask_value = mask->readByID(id);
        if ((int) mask_value == NODATA) {
            //LOG(INFO)<<"NO MASK";
            return -1;
        }
        float env_value = envs[item.first];
        if ((int) env_value == NODATA) {
            //LOG(INFO)<<"NO DATA";
            return -1;
        }
        //LOG(INFO)<<"env_value:"<<env_value <<" MAX:"<<item.second->getMax()<<" MIN:"<<item.second->getMin();
        if ((env_value > item.second->getMax())
                || (env_value < item.second->getMin())) {
            //LOG(INFO)<<"UNSUITABLE";
            return 0;
        }
    }
    //LOG(INFO)<<"SUITABLE";
    return 1;
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
