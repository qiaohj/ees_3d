/**
 * @file Species.cpp
 * @brief Class Species. A class to handle the attributes and behaviors of a virtual species
 * @author Huijie Qiao
 * @version 1.0
 * @date 3/6/2020
 * @details
 * Copyright 2014-2020 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 */

#include "Species.h"

Species::Species(sqlite3_stmt *stmt, int burn_in_year) {
    seeds = new set<int>();
    int from = sqlite3_column_int(stmt, SIMULATION_from);
    int to = sqlite3_column_int(stmt, SIMULATION_to);
    int step = sqlite3_column_int(stmt, SIMULATION_step);
    timeLine = new vector<int>();
    if (from < to) {
        for (int i = from; i <= to; i += step) {
            this->timeLine->push_back(i);
        }
    } else {
        for (int i = from; i >= to; i += step) {
            this->timeLine->push_back(i);
        }
    }
	currentSpeciesExtinctionTimeSteps = 0;
	newSpecies = true;
    id = sqlite3_column_int(stmt, SIMULATION_id);
    vector<string> *dispersalAbility_str = new vector<string>();
    CommonFun::splitStr(
            string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, SIMULATION_dispersal_ability))), ",", dispersalAbility_str);

    dispersalAbilityLength = dispersalAbility_str->size();
    dispersalAbility = new double[dispersalAbilityLength];
	for (int index = 0; index < dispersalAbilityLength; ++index) {
		dispersalAbility[index] = stod(dispersalAbility_str->at(index));
	}
	delete dispersalAbility_str;
	this->burninYear = burn_in_year;

    dispersalSpeed = sqlite3_column_int(stmt, SIMULATION_dispersal_speed);
    dispersalMethod = sqlite3_column_int(stmt, SIMULATION_dispersal_method);
    numberOfPath = sqlite3_column_int(stmt, SIMULATION_number_of_path);
    speciationYears = sqlite3_column_int(stmt, SIMULATION_speciation_years);

    speciesExtinctionThreshold = (unsigned)sqlite3_column_int(stmt, SIMULATION_species_extinction_threshold);
    groupExtinctionThreshold = sqlite3_column_int(stmt, SIMULATION_group_extinction_threshold);
    speciesExtinctionTimeSteps = sqlite3_column_int(stmt, SIMULATION_species_extinction_time_steps);
    speciesExtinctionThreaholdPercentage = 1 - sqlite3_column_double(stmt, SIMULATION_species_extinction_threahold_percentage);;
    //LOG(INFO)<<"speciesExtinctionThreaholdPercentage"<<speciesExtinctionThreaholdPercentage;
    maxSpeciesDistribution = 0;
    appearedYearI = 0;
    disappearedYearI = 0;
    parent = NULL;
    clade_extinction_status = 0;
    number_of_clade_extinction = 0;
    number_of_speciation = 0;
    number_of_species_extinction = 0;
    environment_labels = new vector<string>();
    CommonFun::splitStr(string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, SIMULATION_environments))), ",", environment_labels);
    vector<string> *niche_breadth_array = new vector<string>();
    CommonFun::splitStr(
            string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, SIMULATION_nb_v))), "|", niche_breadth_array);
    nicheBreadth = new unordered_map<string, NicheBreadth*>();
    for (unsigned i = 0; i < niche_breadth_array->size(); ++i) {
        vector<string> *niche_breadth_item = new vector<string>();
        CommonFun::splitStr(niche_breadth_array->at(i), ",", niche_breadth_item);
        NicheBreadth* niche_breadth = new NicheBreadth(stod(niche_breadth_item->at(0)), stod(niche_breadth_item->at(1)));
        nicheBreadth->insert({environment_labels->at(i), niche_breadth});
        delete niche_breadth_item;
    }
    delete niche_breadth_array;

    int initial_seed = sqlite3_column_int(stmt, SIMULATION_initial_seeds);;
    seeds->insert(initial_seed);
}

string Species::getIDWithParentID(){
    if (parent==NULL){
        string id_str = to_string(id);
        return id_str;
    }else{
        string parent_id = parent->getIDWithParentID();
        string id_str = parent_id + "-" + to_string(id);
        return id_str;
    }
}
Species::Species(int p_id, Species* p_parent, int p_year_i) {
    LOG(DEBUG)<<"y1";
    timeLine = new vector<int>();
    for (auto it : *(p_parent->getTimeLine())){
        timeLine->push_back(it);
    }
    LOG(DEBUG)<<"y2";
	currentSpeciesExtinctionTimeSteps = 0;

    newSpecies = true;
    parent = p_parent;
    id = p_id;
    LOG(DEBUG)<<"y3";
    speciesExtinctionThreshold = parent->getSpeciesExtinctionThreshold();
    LOG(DEBUG)<<"y4";
    groupExtinctionThreshold = parent->getGroupExtinctionThreshold();
    LOG(DEBUG)<<"y5";
    speciesExtinctionTimeSteps = p_parent->getSpeciesExtinctionTimeSteps();
    LOG(DEBUG)<<"y6";
    speciesExtinctionThreaholdPercentage = p_parent->getSpeciesExtinctionThreaholdPercentage();
    maxSpeciesDistribution = 0;
    LOG(DEBUG)<<"y7";
    dispersalAbility = new double[parent->getDispersalAbilityLength()];
    LOG(DEBUG)<<"y8";
    for (int i=0; i<parent->getDispersalAbilityLength();i++){
        dispersalAbility[i] = parent->getDispersalAbility()[i];
    }
    dispersalSpeed = parent->getDispersalSpeed();
    dispersalMethod = parent->getDispersalMethod();
    numberOfPath = parent->getNumOfPath();
    speciationYears = parent->getSpeciationYears();
    LOG(DEBUG)<<"y9";
    nicheBreadth = new unordered_map<string, NicheBreadth*>();
    for (auto it : *(parent->getNicheBreadth())){
        NicheBreadth* p_NicheBreadth = it.second;
        NicheBreadth* new_NicheBreadth = new NicheBreadth(p_NicheBreadth->getMin(), p_NicheBreadth->getMax());
        nicheBreadth->insert({it.first, new_NicheBreadth});
    }
    LOG(DEBUG)<<"y10";
    dispersalAbilityLength = parent->getDispersalAbilityLength();
    burninYear = parent->getBurnInYear();

    parent->setDisappearedYearI(p_year_i);
    appearedYearI = p_year_i;
    disappearedYearI = 0;
    LOG(DEBUG)<<"y11";
    parent->addChild(this);
    clade_extinction_status = 0;
    number_of_clade_extinction = 0;
    number_of_speciation = 0;
    number_of_species_extinction = 0;
    LOG(DEBUG)<<"y12";
    environment_labels = new vector<string>();
    for (auto it : *(parent->getEnvironmentLabels())){
        environment_labels->push_back(it);
    }
    seeds = new set<int>();
    for (auto it : *(parent->getSeeds())){
        seeds->insert(it);
    }
}
vector<string> *Species::getEnvironmentLabels(){
    return this->environment_labels;
}
void Species::setCladeExtinctionStatus(int status) {
    clade_extinction_status = status;
}
void Species::markParentClade() {
    for (auto child : children) {
        child->setCladeExtinctionStatus(3);
        child->markParentClade();
    }
}
string Species::getSpeciationExtinction(bool isroot,
        int total_years) {
    char t_char[100];
    sprintf(t_char, "clade_extinction,species_extinction,speciation\n%u,%u,%u,%u",
    		total_years, number_of_clade_extinction, number_of_species_extinction,
            number_of_speciation);
    return string(t_char);
}
bool Species::isAllLeafExtinction(int total_years) {
    bool is_extinction = true;
    for (auto child : children) {
        if (child->getChildren().size() == 0) {
            if (child->getDisappearedYearI() == total_years) {
                return false;
            }
        } else {
            is_extinction = child->isAllLeafExtinction(total_years);
            if (!is_extinction) {
                return is_extinction;
            }
        }
    }
    return true;
}
int Species::getNumberOfCladeExtinction() {
    return number_of_clade_extinction;
}
int Species::getNumberOfSpeciation() {
    return number_of_speciation;
}
int Species::getNumberOfSpeciesExtinction() {
    return number_of_species_extinction;
}
void Species::markNode(int total_years) {
    number_of_clade_extinction = 0;
    number_of_speciation = 0;
    number_of_species_extinction = 0;

    //if the status is unknown, try to mark the status
    if (clade_extinction_status == 0) {
        bool allleaf_extinction = isAllLeafExtinction(total_years);
        if (allleaf_extinction) {
            //clade extinction
            clade_extinction_status = 1;
            //mark all the children as parent_clade extinction
            markParentClade();
            number_of_clade_extinction = 1;
        } else {
            //clade unextiction
            clade_extinction_status = 2;
        }
    }
    for (auto child : children) {
        child->markNode(total_years);
    }

    if (children.size() == 0) {
        number_of_clade_extinction = 0;
        number_of_speciation = 0;
        number_of_species_extinction = ((disappearedYearI == 0) || (disappearedYearI == total_years)) ? 0 : 1;
    } else {
        for (auto child : children) {
            number_of_clade_extinction += child->getNumberOfCladeExtinction();
            number_of_speciation += child->getNumberOfSpeciation();
            number_of_species_extinction += child->getNumberOfSpeciesExtinction();
        }
        number_of_speciation++;
    }
}
vector<string> Species::getHTMLTree(int p_year) {
    vector<string> html_output;
    html_output.push_back(
            "<!DOCTYPE html><html lang=\"en\" xml:lang=\"en\" xmlns=\"http://www.w3.org/1999/xhtml\">");
    html_output.push_back(
            "<head><meta content=\"text/html;charset=UTF-8\" http-equiv=\"content-type\"><title>Phylogram Tree</title>");
    html_output.push_back(
            "<script src=\"d3.v3.min.js\" type=\"text/javascript\"></script>");
    html_output.push_back(
            "<script src=\"newick.js\" type=\"text/javascript\"></script>");
    html_output.push_back(
            "<script src=\"d3.phylogram.js\" type=\"text/javascript\"></script>");
    html_output.push_back("<script>");
    html_output.push_back("function load() {");
    html_output.push_back(
            "var newick = Newick.parse(\"" + getNewickTree(true, true, p_year) + "\");");
    html_output.push_back("var newickNodes = [];");
    html_output.push_back("function buildNewickNodes(node, callback) {");
    html_output.push_back("newickNodes.push(node);");
    html_output.push_back("if (node.branchset) {");
    html_output.push_back("for (var i=0; i < node.branchset.length; i++) {");
    html_output.push_back("buildNewickNodes(node.branchset[i])");
    html_output.push_back("}");
    html_output.push_back("}");
    html_output.push_back("};");
    html_output.push_back("buildNewickNodes(newick);");
    html_output.push_back(
            "          d3.phylogram.build(\"#phylogram\", newick, {");
    html_output.push_back("width: 1200,");
    html_output.push_back("height: 300");
    html_output.push_back("});");
    html_output.push_back("}");
    html_output.push_back("</script>");
    html_output.push_back("<style type=\"text/css\" media=\"screen\">");
    html_output.push_back(
            "body { font-family: \"Helvetica Neue\", Helvetica, sans-serif; }");
    html_output.push_back("td { vertical-align: top; }");
    html_output.push_back("</style>");
    html_output.push_back("</head>");
    html_output.push_back("<body onload=\"load()\">");
    html_output.push_back("<table>");
    html_output.push_back("<tr>");
    html_output.push_back("<td>");
    html_output.push_back("<h2>Phylogram tree</h2>");
    html_output.push_back("<div id=\"phylogram\"></div>");
    html_output.push_back("</td>");

    html_output.push_back("      </tr>");
    html_output.push_back("</table>");
    html_output.push_back("</body>");
    html_output.push_back("</html>");
    return html_output;
}
string Species::getNewickTree(bool isroot, bool iscolor, int p_year_i) {
    string output = "";
    unsigned i = 0;
    if (children.size() > 0) {
        output += "(";
        for (auto child : children) {
            i++;
            output += child->getNewickTree(false, iscolor, p_year_i);
            if (i < children.size()) {
                output += ",";
            }

        }
        output += ")";
    }
    string color = "white";
    if (children.size() > 0) {
        color = "red";
    } else {
        if ((number_of_species_extinction == 1)) {
            color = "blue";
        }
    }
    string linecolor = "#ccc";
    if ((clade_extinction_status == 1) || (clade_extinction_status == 3)) {
        linecolor = "red";
    }
    int t_disappearedYear = (disappearedYearI==0)?p_year_i:disappearedYearI;
    int parent_year = (parent == NULL) ? 0 : parent->getDisappearedYearI();
    if (appearedYearI == t_disappearedYear) {
        if (iscolor) {
            char t_char[100];
            sprintf(t_char, "SP%u[%u]:%u@%s~%s", id, timeLine->at(appearedYearI),
                    timeLine->at(t_disappearedYear - parent_year), color.c_str(),
                    linecolor.c_str());
            output += string(t_char);
        } else {
            char t_char[100];
            sprintf(t_char, "SP%d[%d]:%d", id, timeLine->at(appearedYearI),
                    timeLine->at(t_disappearedYear - parent_year));
            output += string(t_char);
        }
    } else {
        if (iscolor) {
            char t_char[100];
            sprintf(t_char, "SP%d[%d-%d]:%d@%s~%s", id, timeLine->at(appearedYearI),
                    timeLine->at(t_disappearedYear), timeLine->at(t_disappearedYear - parent_year),
                    color.c_str(), linecolor.c_str());
            output += string(t_char);
        } else {
            char t_char[100];
            sprintf(t_char, "SP%d[%d-%d]:%d", id, timeLine->at(appearedYearI), timeLine->at(t_disappearedYear),
                    timeLine->at(t_disappearedYear - parent_year));
            output += string(t_char);
        }
    }
    if (isroot) {
        output += ";";
    }
    return output;
}
vector<Species*> Species::getChildren() {
    return children;
}
void Species::addChild(Species* child) {
    children.push_back(child);
}
void Species::setDisappearedYearI(int p_disappeared_year_i) {
    disappearedYearI = p_disappeared_year_i;
}
int Species::getDisappearedYearI() {
    return disappearedYearI;
}
int Species::getAppearedYearI() {
    return appearedYearI;
}
int Species::getBurnInYear() {
    return this->burninYear;
}

Species::~Species() {
    LOG(DEBUG)<<"w1";
    delete[] dispersalAbility;
    LOG(DEBUG)<<"w2";
    for (auto it : *nicheBreadth){
        LOG(DEBUG)<<"w3";
        delete it.second;
    }
    LOG(DEBUG)<<"w4";
    delete nicheBreadth;
    LOG(DEBUG)<<"w5";
    delete seeds;
    LOG(DEBUG)<<"w6";
    delete environment_labels;
    LOG(DEBUG)<<"w7";
    delete timeLine;
}
int Species::getDispersalAbilityLength(){
	return dispersalAbilityLength;
}
double* Species::getDispersalAbility() {
    return dispersalAbility;
}
vector<int> *Species::getTimeLine(){
    return this->timeLine;
}

unsigned Species::getSpeciesExtinctionThreshold(){
	return speciesExtinctionThreshold;
}

int Species::getGroupExtinctionThreshold(){
	return groupExtinctionThreshold;
}

int Species::getSpeciesExtinctionTimeSteps(){
	return speciesExtinctionTimeSteps;
}

double Species::getSpeciesExtinctionThreaholdPercentage(){
	return speciesExtinctionThreaholdPercentage;
}
void Species::setMaxSpeciesDistribution(unsigned p_max_species_distribution){
	maxSpeciesDistribution = p_max_species_distribution;
}
unsigned Species::getMaxSpeciesDistribution(){
	return maxSpeciesDistribution;
}
int Species::getCurrentSpeciesExtinctionTimeSteps(){
	return currentSpeciesExtinctionTimeSteps;
}

void Species::setCurrentSpeciesExtinctionTimeSteps(int p_currentSpeciesExtinctionTimeSteps){
	currentSpeciesExtinctionTimeSteps = p_currentSpeciesExtinctionTimeSteps;
}

void Species::addCurrentSpeciesExtinctionTimeSteps(){
	currentSpeciesExtinctionTimeSteps++;
}


int Species::getDispersalSpeed() {
    return dispersalSpeed;
}
set<int> *Species::getSeeds() {
    return seeds;
}
int Species::getID() {
    return id;
}
int Species::getDispersalMethod() {
    return dispersalMethod;
}
int Species::getNumOfPath() {
    return numberOfPath;
}
unordered_map<string, NicheBreadth*> *Species::getNicheBreadth() {
    return nicheBreadth;
}
int Species::getSpeciationYears() {
	//return 99999999;
    return speciationYears;
}
Species* Species::getParent() {
    return parent;
}
bool Species::isNewSpecies(){
    return newSpecies;
}
void Species::setNewSpecies(bool p){
    newSpecies = p;
}
