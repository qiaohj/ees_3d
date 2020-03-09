/**
 * @file SpeciesObject3D.cpp
 * @brief Class SpeciesObject3D. A class to handle the attributes and behaviors of a virtual species
 * @author Huijie Qiao
 * @version 1.0
 * @date 3/6/2020
 * @details
 * Copyright 2014-2020 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 */

#include "SpeciesObject3D.h"

SpeciesObject3D::SpeciesObject3D(sqlite3_stmt *stmt, int burn_in_year, vector<int>& timeLine) {
    this->timeLine = timeLine;
	currentSpeciesExtinctionTimeSteps = 0;
	newSpecies = true;
    id = sqlite3_column_int(stmt, SIMULATION_id);
    vector<string> dispersalAbility_str = CommonFun::splitStr(
            string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, SIMULATION_dispersal_ability))), ",");

    dispersalAbilityLength = dispersalAbility_str.size();
    dispersalAbility = new double[dispersalAbilityLength];
	for (int index = 0; index < dispersalAbilityLength; ++index) {
		dispersalAbility[index] = stod(dispersalAbility_str[index]);
	}
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
    environment_labels = CommonFun::splitStr(string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, SIMULATION_environments))), ",");
    vector<string> niche_breadth_array = CommonFun::splitStr(
            string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, SIMULATION_nb_v))), "|");
    for (unsigned i = 0; i < niche_breadth_array.size(); ++i) {
        vector<string> niche_breadth_item = CommonFun::splitStr(niche_breadth_array[i], ",");
        NicheBreadth* niche_breadth = new NicheBreadth(stod(niche_breadth_item[0]), stod(niche_breadth_item[1]));
        nicheBreadth[environment_labels[i]] = niche_breadth;
    }

    int initial_seed = sqlite3_column_int(stmt, SIMULATION_initial_seeds);;
    seeds.insert(initial_seed);
}

string SpeciesObject3D::getIDWithParentID(){
    if (parent==NULL){
        char t_char[5];
        sprintf(t_char, "%u", id);
        return string(t_char);
    }else{
        string parent_id = parent->getIDWithParentID();
        char t_char[parent_id.length() + 5];
        sprintf(t_char, "%s-%u", parent_id.c_str(), id);
        return string(t_char);
    }
}
SpeciesObject3D::SpeciesObject3D(int p_id, SpeciesObject3D* p_parent, int p_year_i) {
    this->timeLine = p_parent->getTimeLine();
	currentSpeciesExtinctionTimeSteps = 0;


    newSpecies = true;
    parent = p_parent;
    id = p_id;
    speciesExtinctionThreshold = parent->getSpeciesExtinctionThreshold();
    groupExtinctionThreshold = parent->getGroupExtinctionThreshold();
    speciesExtinctionTimeSteps = p_parent->getSpeciesExtinctionTimeSteps();
    speciesExtinctionThreaholdPercentage = p_parent->getSpeciesExtinctionThreaholdPercentage();
    maxSpeciesDistribution = 0;
    dispersalAbility = parent->getDispersalAbility();
    dispersalSpeed = parent->getDispersalSpeed();
    dispersalMethod = parent->getDispersalMethod();
    numberOfPath = parent->getNumOfPath();
    speciationYears = parent->getSpeciationYears();
    nicheBreadth = parent->getNicheBreadth();
    parent->setDisappearedYearI(p_year_i);
    appearedYearI = p_year_i;
    disappearedYearI = 0;
    parent->addChild(this);
    clade_extinction_status = 0;
    number_of_clade_extinction = 0;
    number_of_speciation = 0;
    number_of_species_extinction = 0;
}
void SpeciesObject3D::setCladeExtinctionStatus(int status) {
    clade_extinction_status = status;
}
void SpeciesObject3D::markParentClade() {
    for (auto child : children) {
        child->setCladeExtinctionStatus(3);
        child->markParentClade();
    }
}
string SpeciesObject3D::getSpeciationExtinction(bool isroot,
        int total_years) {
    char t_char[100];
    sprintf(t_char, "clade_extinction,species_extinction,speciation\n%u,%u,%u,%u",
    		total_years, number_of_clade_extinction, number_of_species_extinction,
            number_of_speciation);
    return string(t_char);
}
bool SpeciesObject3D::isAllLeafExtinction(int total_years) {
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
int SpeciesObject3D::getNumberOfCladeExtinction() {
    return number_of_clade_extinction;
}
int SpeciesObject3D::getNumberOfSpeciation() {
    return number_of_speciation;
}
int SpeciesObject3D::getNumberOfSpeciesExtinction() {
    return number_of_species_extinction;
}
void SpeciesObject3D::markNode(int total_years) {
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
vector<string> SpeciesObject3D::getHTMLTree(int p_year) {
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
string SpeciesObject3D::getNewickTree(bool isroot, bool iscolor, int p_year_i) {
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
            char t_char[40];
            sprintf(t_char, "SP%u[%u]:%u@%s~%s", id, timeLine[appearedYearI],
                    timeLine[t_disappearedYear - parent_year], color.c_str(),
                    linecolor.c_str());
            output += string(t_char);
        } else {
            char t_char[40];
            sprintf(t_char, "SP%u[%u]:%u", id, timeLine[appearedYearI],
                    timeLine[t_disappearedYear - parent_year]);
            output += string(t_char);
        }
    } else {
        if (iscolor) {
            char t_char[40];
            sprintf(t_char, "SP%u[%u-%u]:%u@%s~%s", id, timeLine[appearedYearI],
                    timeLine[t_disappearedYear], timeLine[t_disappearedYear - parent_year],
                    color.c_str(), linecolor.c_str());
            output += string(t_char);
        } else {
            char t_char[40];
            sprintf(t_char, "SP%u[%u-%u]:%u", id, timeLine[appearedYearI], timeLine[t_disappearedYear],
                    timeLine[t_disappearedYear - parent_year]);
            output += string(t_char);
        }
    }
    if (isroot) {
        output += ";";
    }
    return output;
}
vector<SpeciesObject3D*> SpeciesObject3D::getChildren() {
    return children;
}
void SpeciesObject3D::addChild(SpeciesObject3D* child) {
    children.push_back(child);
}
void SpeciesObject3D::setDisappearedYearI(int p_disappeared_year_i) {
    disappearedYearI = p_disappeared_year_i;
}
int SpeciesObject3D::getDisappearedYearI() {
    return disappearedYearI;
}
int SpeciesObject3D::getAppearedYearI() {
    return appearedYearI;
}
int SpeciesObject3D::getBurnInYear() {
    return this->burninYear;
}

SpeciesObject3D::~SpeciesObject3D() {
    //CommonFun::clearUnordered_map);
    seeds.clear();
}
int SpeciesObject3D::getDispersalAbilityLength(){
	return dispersalAbilityLength;
}
double* SpeciesObject3D::getDispersalAbility() {
    return dispersalAbility;
}
vector<int> SpeciesObject3D::getTimeLine(){
    return this->timeLine;
}

unsigned SpeciesObject3D::getSpeciesExtinctionThreshold(){
	return speciesExtinctionThreshold;
}

int SpeciesObject3D::getGroupExtinctionThreshold(){
	return groupExtinctionThreshold;
}

int SpeciesObject3D::getSpeciesExtinctionTimeSteps(){
	return speciesExtinctionTimeSteps;
}

double SpeciesObject3D::getSpeciesExtinctionThreaholdPercentage(){
	return speciesExtinctionThreaholdPercentage;
}
void SpeciesObject3D::setMaxSpeciesDistribution(unsigned p_max_species_distribution){
	maxSpeciesDistribution = p_max_species_distribution;
}
unsigned SpeciesObject3D::getMaxSpeciesDistribution(){
	return maxSpeciesDistribution;
}
int SpeciesObject3D::getCurrentSpeciesExtinctionTimeSteps(){
	return currentSpeciesExtinctionTimeSteps;
}

void SpeciesObject3D::setCurrentSpeciesExtinctionTimeSteps(int p_currentSpeciesExtinctionTimeSteps){
	currentSpeciesExtinctionTimeSteps = p_currentSpeciesExtinctionTimeSteps;
}

void SpeciesObject3D::addCurrentSpeciesExtinctionTimeSteps(){
	currentSpeciesExtinctionTimeSteps++;
}


int SpeciesObject3D::getDispersalSpeed() {
    return dispersalSpeed;
}
set<int> SpeciesObject3D::getSeeds() {
    return seeds;
}
int SpeciesObject3D::getID() {
    return id;
}
int SpeciesObject3D::getDispersalMethod() {
    return dispersalMethod;
}
int SpeciesObject3D::getNumOfPath() {
    return numberOfPath;
}
boost::unordered_map<string, NicheBreadth*> SpeciesObject3D::getNicheBreadth() {
    return nicheBreadth;
}
int SpeciesObject3D::getSpeciationYears() {
	//return 99999999;
    return speciationYears;
}
SpeciesObject3D* SpeciesObject3D::getParent() {
    return parent;
}
bool SpeciesObject3D::isNewSpecies(){
    return newSpecies;
}
void SpeciesObject3D::setNewSpecies(bool p){
    newSpecies = p;
}
