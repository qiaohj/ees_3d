/**
 * @file SpeciesObject3D.cpp
 * @brief Class SpeciesObject3D. A class to handle the attributes and behaviors of a virtual species
 * @author Huijie Qiao
 * @version 1.0
 * @date 11/25/2018
 * @details
 * Copyright 2014-2019 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 */

#include "SpeciesObject3D.h"

SpeciesObject3D::SpeciesObject3D(const string json_path) {
	currentSpeciesExtinctionTimeSteps = 0;
	//LOG(INFO)<<"Load species configure from " <<json_path;
	Json::Value species_json = CommonFun::readJson(json_path.c_str());
    newSpecies = true;
    id = species_json.get("id", "").asInt();
    Json::Value dispersal_ability_array = species_json["dispersal_ability"];
    dispersalAbilityLength = dispersal_ability_array.size();
    dispersalAbility = new double[10];
	for (unsigned index = 0; index < dispersalAbilityLength; ++index) {
		dispersalAbility[index] = dispersal_ability_array[index].asDouble();
	}

    dispersalSpeed = species_json.get("dispersal_speed", 100).asInt();
    dispersalMethod = species_json.get("dispersal_method", 2).asInt();
    numberOfPath = species_json.get("number_of_path", -1).asInt();
    speciationYears = species_json.get("speciation_years", 10000).asInt();

    speciesExtinctionThreshold = species_json.get("species_extinction_threshold", 1).asInt();
    groupExtinctionThreshold = species_json.get("group_extinction_threshold", 1).asInt();
    speciesExtinctionTimeSteps = species_json.get("species_extinction_time_steps", 1).asInt();
    speciesExtinctionThreaholdPercentage = 1 - species_json.get("species_extinction_threahold_percentage", 1).asDouble();
    //LOG(INFO)<<"speciesExtinctionThreaholdPercentage"<<speciesExtinctionThreaholdPercentage;
    maxSpeciesDistribution = 0;
    appearedYear = 0;
    disappearedYear = 0;
    parent = NULL;
    clade_extinction_status = 0;
    number_of_clade_extinction = 0;
    number_of_speciation = 0;
    number_of_species_extinction = 0;
    Json::Value niche_breadth_array = species_json["niche_breadth"];
    for (unsigned index = 0; index < niche_breadth_array.size(); ++index) {
        Json::Value niche_breadth_json = niche_breadth_array[index];
        NicheBreadth* niche_breadth = new NicheBreadth(
                niche_breadth_json[0].asFloat(),
                niche_breadth_json[1].asFloat());
        nicheBreadth.push_back(niche_breadth);
    }

    Json::Value initial_seeds_array = species_json["initial_seeds"];
    for (unsigned index = 0; index < initial_seeds_array.size(); ++index) {
        Json::Value initial_seeds_json = initial_seeds_array[index];
        //LOG(INFO)<<initial_seeds_json;
        unsigned initial_seed = (unsigned)initial_seeds_json.asInt();
        seeds.insert(initial_seed);
    }
    LOG(INFO)<<"finished";
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
SpeciesObject3D::SpeciesObject3D(unsigned p_id, SpeciesObject3D* p_parent,
        unsigned p_year) {
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
    parent->setDisappearedYear(p_year);
    appearedYear = p_year;
    disappearedYear = 0;
    parent->addChild(this);
    clade_extinction_status = 0;
    number_of_clade_extinction = 0;
    number_of_speciation = 0;
    number_of_species_extinction = 0;
}
void SpeciesObject3D::setCladeExtinctionStatus(unsigned status) {
    clade_extinction_status = status;
}
void SpeciesObject3D::markParentClade() {
    for (auto child : children) {
        child->setCladeExtinctionStatus(3);
        child->markParentClade();
    }
}
string SpeciesObject3D::getSpeciationExtinction(bool isroot,
        unsigned total_years) {
    char t_char[100];
    sprintf(t_char, "clade_extinction,species_extinction,speciation\n%u,%u,%u,%u",
    		total_years, number_of_clade_extinction, number_of_species_extinction,
            number_of_speciation);
    return string(t_char);
}
bool SpeciesObject3D::isAllLeafExtinction(unsigned total_years) {
    bool is_extinction = true;
    for (auto child : children) {
        if (child->getChildren().size() == 0) {
            if (child->getDisappearedYear() == total_years) {
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
unsigned SpeciesObject3D::getNumberOfCladeExtinction() {
    return number_of_clade_extinction;
}
unsigned SpeciesObject3D::getNumberOfSpeciation() {
    return number_of_speciation;
}
unsigned SpeciesObject3D::getNumberOfSpeciesExtinction() {
    return number_of_species_extinction;
}
void SpeciesObject3D::markNode(unsigned total_years) {
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
        number_of_species_extinction = ((disappearedYear==0)||(disappearedYear == total_years)) ? 0 : 1;
    } else {
        for (auto child : children) {
            number_of_clade_extinction += child->getNumberOfCladeExtinction();
            number_of_speciation += child->getNumberOfSpeciation();
            number_of_species_extinction +=
                    child->getNumberOfSpeciesExtinction();
        }
        number_of_speciation++;
    }
}
vector<string> SpeciesObject3D::getHTMLTree(unsigned p_year) {
    vector<string> html_output;
    html_output.push_back(
            "<!DOCTYPE html><html lang='en' xml:lang='en' xmlns='http://www.w3.org/1999/xhtml'>");
    html_output.push_back(
            "<head><meta content='text/html;charset=UTF-8' http-equiv='content-type'><title>Phylogram Tree</title>");
    html_output.push_back(
            "<script src='d3.v3.min.js' type='text/javascript'></script>");
    html_output.push_back(
            "<script src='newick.js' type='text/javascript'></script>");
    html_output.push_back(
            "<script src='d3.phylogram.js' type='text/javascript'></script>");
    html_output.push_back("<script>");
    html_output.push_back("function load() {");
    html_output.push_back(
            "var newick = Newick.parse('" + getNewickTree(true, true, p_year) + "');");
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
            "          d3.phylogram.build('#phylogram', newick, {");
    html_output.push_back("width: 1200,");
    html_output.push_back("height: 300");
    html_output.push_back("});");
    html_output.push_back("}");
    html_output.push_back("</script>");
    html_output.push_back("<style type='text/css' media='screen'>");
    html_output.push_back(
            "body { font-family: 'Helvetica Neue', Helvetica, sans-serif; }");
    html_output.push_back("td { vertical-align: top; }");
    html_output.push_back("</style>");
    html_output.push_back("</head>");
    html_output.push_back("<body onload='load()'>");
    html_output.push_back("<table>");
    html_output.push_back("<tr>");
    html_output.push_back("<td>");
    html_output.push_back("<h2>Phylogram tree</h2>");
    html_output.push_back("<div id='phylogram'></div>");
    html_output.push_back("</td>");

    html_output.push_back("      </tr>");
    html_output.push_back("</table>");
    html_output.push_back("</body>");
    html_output.push_back("</html>");
    return html_output;
}
string SpeciesObject3D::getNewickTree(bool isroot, bool iscolor, unsigned p_year) {
    string output = "";
    unsigned i = 0;
    if (children.size() > 0) {
        output += "(";
        for (auto child : children) {
            i++;
            output += child->getNewickTree(false, iscolor, p_year);
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
    unsigned t_disappearedYear = (disappearedYear==0)?p_year:disappearedYear;
    int parent_year = (parent == NULL) ? 0 : parent->getDisappearedYear();
    if (appearedYear == t_disappearedYear) {
        if (iscolor) {
            char t_char[40];
            sprintf(t_char, "SP%u[%u]:%u@%s~%s", id, appearedYear,
                    t_disappearedYear - parent_year, color.c_str(),
                    linecolor.c_str());
            output += string(t_char);
        } else {
            char t_char[40];
            sprintf(t_char, "SP%u[%u]:%u", id, appearedYear,
                    t_disappearedYear - parent_year);
            output += string(t_char);
        }
    } else {
        if (iscolor) {
            char t_char[40];
            sprintf(t_char, "SP%u[%u-%u]:%u@%s~%s", id, appearedYear,
                    t_disappearedYear, t_disappearedYear - parent_year,
                    color.c_str(), linecolor.c_str());
            output += string(t_char);
        } else {
            char t_char[40];
            sprintf(t_char, "SP%u[%u-%u]:%u", id, appearedYear, t_disappearedYear,
                    t_disappearedYear - parent_year);
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
void SpeciesObject3D::setDisappearedYear(unsigned p_disappeared_year) {
    disappearedYear = p_disappeared_year;
}
unsigned SpeciesObject3D::getDisappearedYear() {
    return disappearedYear;
}
unsigned SpeciesObject3D::getAppearedYear() {
    return appearedYear;
}
SpeciesObject3D::~SpeciesObject3D() {
    CommonFun::clearVector(&nicheBreadth);
    seeds.clear();
}
unsigned short SpeciesObject3D::getDispersalAbilityLength(){
	return dispersalAbilityLength;
}
double* SpeciesObject3D::getDispersalAbility() {
    return dispersalAbility;
}

unsigned SpeciesObject3D::getSpeciesExtinctionThreshold(){
	return speciesExtinctionThreshold;
}

unsigned SpeciesObject3D::getGroupExtinctionThreshold(){
	return groupExtinctionThreshold;
}

unsigned SpeciesObject3D::getSpeciesExtinctionTimeSteps(){
	return speciesExtinctionTimeSteps;
}

double SpeciesObject3D::getSpeciesExtinctionThreaholdPercentage(){
	return speciesExtinctionThreaholdPercentage;
}
void SpeciesObject3D::setMaxSpeciesDistribution(unsigned distribution){
	maxSpeciesDistribution = distribution;
}
unsigned SpeciesObject3D::getMaxSpeciesDistribution(){
	return maxSpeciesDistribution;
}
unsigned SpeciesObject3D::getCurrentSpeciesExtinctionTimeSteps(){
	return currentSpeciesExtinctionTimeSteps;
}

void SpeciesObject3D::setCurrentSpeciesExtinctionTimeSteps(unsigned p_currentSpeciesExtinctionTimeSteps){
	currentSpeciesExtinctionTimeSteps = p_currentSpeciesExtinctionTimeSteps;
}

void SpeciesObject3D::addCurrentSpeciesExtinctionTimeSteps(){
	currentSpeciesExtinctionTimeSteps++;
}


unsigned SpeciesObject3D::getDispersalSpeed() {
    return dispersalSpeed;
}
set<int> SpeciesObject3D::getSeeds() {
    return seeds;
}
unsigned SpeciesObject3D::getID() {
    return id;
}
int SpeciesObject3D::getDispersalMethod() {
    return dispersalMethod;
}
int SpeciesObject3D::getNumOfPath() {
    return numberOfPath;
}
vector<NicheBreadth*> SpeciesObject3D::getNicheBreadth() {
    return nicheBreadth;
}
unsigned SpeciesObject3D::getSpeciationYears() {
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