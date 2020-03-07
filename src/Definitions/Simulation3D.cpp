/**
 * @file Simulation3D.cpp
 * @brief Class Simulation3D. A class to define the features of a virtual scenario in a 3D simulation, and the virtual species in the scenario.
 * @author Huijie Qiao
 * @version 1.0
 * @date 3/7/2020
 * @details
 * Copyright 2014-2020 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 */
#include "Simulation3D.h"
Simulation3D::Simulation3D() {

}

void Simulation3D::setNeighbor(Neighbor3D *neighborInfo){
    this->neighborInfo = neighborInfo;
}
void Simulation3D::setOverwrite(bool p_overwrite){
    this->overwrite = p_overwrite;
}
bool Simulation3D::getOverwrite(){
    return this->overwrite;
}

/*-------------------------
 * Save the population information for a specific time step to a db
 *-----------------------*/
void Simulation3D::saveGroupmap(int year, boost::unordered_map<SpeciesObject3D*, ISEA3H*> species_group_maps) {
    LOG(DEBUG)<<"Save result to db";
    if (species_group_maps.size() == 0) {
        return;
    }
    vector<string> output;
    char line[100];
    // Note: The old version has only 5 columns without lon and lat columns.
    sprintf(line, "BEGIN TRANSACTION; insert into map (YEAR, ID, group_id, sp_id ) values ");
    output.push_back(line);
    int i = 0;
    for (auto sp_it : species_group_maps) {
        SpeciesObject3D *sp = sp_it.first;
        ISEA3H *map = sp_it.second;
        if (map) {
            for (auto item : map->getValues()) {
                int id = item.first;
                int v = (int) item.second;
                if (v > 0) {
                    string sp_id = sp->getIDWithParentID();
                    char line[sp_id.size() + 100];
                    if (i == 0) {
                        sprintf(line, " (%u,%u,%u,%s) ", year, id, v, sp_id.c_str());
                    } else {
                        sprintf(line, " ,(%u,%u,%u,%s) ", year, id, v, sp_id.c_str());
                    }
                    i = 1;
                    output.push_back(line);
                }
            }

        }
    }
    output.push_back("; COMMIT;");
    if (output.size() > 0) {
        CommonFun::executeSQL(output, log_db);
        output.clear();
    }
}

void Simulation3D::setTotalYears(int p_total_years){
    this->total_years = p_total_years;
}
int Simulation3D::getTotalYears(){
    return this->total_years;
}
void Simulation3D::setTargetFolder(string p_target){
    this->targetFolder = p_target;

}
string Simulation3D::getTargetFolder(){
    return this->targetFolder;
}
void Simulation3D::addEnvironment(string environment_label, EnvironmentalISEA3H *env) {
    this->environments[environment_label] = env;
}
void Simulation3D::addEnvironmentLabel(string environment_label) {
    this->environment_labels.push_back(environment_label);
}
void Simulation3D::addSpecies(SpeciesObject3D *species) {
    this->species.push_back(species);
}
void Simulation3D::setTimeLine(int from, int to, int step) {
    if (from < to) {
        for (int i = from; i <= to; i+=step) {
            this->timeLine.push_back(i);
        }
    } else {
        for (int i = from; i >= to; i+=step) {
            this->timeLine.push_back(i);
        }
    }
}
void Simulation3D::createLogDB() {
    this->logFile = targetFolder + "/" + this->species.front()->getLabel() + ".sqlite";
    int rc;
    rc = sqlite3_open(logFile.c_str(), &log_db);
    if (rc) {
        LOG(INFO) << "Can't open log database: " << sqlite3_errmsg(log_db) << " at " << logFile;
        exit(0);
    } else {
        LOG(INFO) << "Create LOG Database at " << logFile << " successfully";
    }
    //char *zErrMsg = 0;
    //Create a table to save the log // year,x,y,lon,lat,group,sp_id
    string sql = "CREATE TABLE map("
            "YEAR       INT             NOT NULL,"
            "ID         INT             NOT NULL,"
            "group_id   INT             NOT NULL,"
            "sp_id      CHAR(50)                );";
    CommonFun::executeSQL(sql, log_db);

}
void Simulation3D::init(){
    SpeciesObject3D* ancestor = this->species.front();
    LOG(DEBUG) << "Save result to " << targetFolder;
    //Create the necessary folders.
    CommonFun::createFolder(targetFolder.c_str());
    CommonFun::deleteFile(logFile.c_str());
    createLogDB();

    int minSpeciesDispersalSpeed = this->total_years;

    //Load the species parameters.

    set<int> seeds = ancestor->getSeeds();
    boost::unordered_map<int, vector<IndividualOrganism3D*> > t_o;
    /* -----------------
     * Create the individual organism(s) based on the seeds in the species' configuration.
     * All the individual organism(s) has(have) the same parameters inherited from the species and the different localities.
     * Now there is only on seed per species.
     * Don't use the function of the multiple species and multiple seeds per species now,
     * because it hasn't be tested strictly.
     *-------------------------*/
    for (int seed : seeds) {
        IndividualOrganism3D *individualOrganism = new IndividualOrganism3D(0, ancestor, NULL, seed);
        boost::unordered_map<int, IndividualOrganism3D*> t;
        t[seed] = individualOrganism;
        t_o[seed].push_back(individualOrganism);

    }
    all_individualOrganisms[0][ancestor] = t_o;
    /*---------------------
     *
     * get the minimal dispersal speed from all the species in the simulation.
     * It is unused parameter now, because there is only one species per scenario.
     * It might be used later.
     *
     -----------------------*/

    minSpeciesDispersalSpeed = (ancestor->getDispersalSpeed() < minSpeciesDispersalSpeed) ? ancestor->getDispersalSpeed() : minSpeciesDispersalSpeed;

}
vector<SpeciesObject3D*> Simulation3D::getSpecies() {
    return this->species;
}
vector<string> Simulation3D::getEnvironmentLabels() {
    return this->environment_labels;
}
EnvironmentalISEA3H* Simulation3D::getEnvironment(string environment_label) {
    return this->environments[environment_label];
}
vector<int> Simulation3D::getTimeLine() {
    return this->timeLine;
}

void Simulation3D::setMask(ISEA3H* p_mask){
    this->mask = p_mask;
}
ISEA3H* Simulation3D::getMask(){
    return this->mask;
}
/*---------------------------
 * Run a simulation on a scenario with the species in the scenario.
 *---------------------*/
int Simulation3D::run() {
    bool isFinished = boost::filesystem::exists(this->targetFolder);

    /*-------------------
     * If the target folder exists and the is_overwrite parameter is false, skip the simulation,
     * or overwrite the existing result with the new simulation.
     -------------------------*/

    if ((isFinished) && (!overwrite)) {
        LOG(INFO) << "Result folder is exist, skip this simulation!";
        return 0;
    }
    boost::unordered_map<SpeciesObject3D*, ISEA3H*> species_group_maps;
    vector<string> stat_output;
    for (unsigned year_i = 1; year_i<timeLine.size(); year_i++) {
        int year = timeLine[year_i];
        LOG(INFO) << "Current year:" << year << " @ " << targetFolder << " Memory usage:" << CommonFun::getCurrentRSS();

        boost::unordered_map<SpeciesObject3D*, boost::unordered_map<int, vector<IndividualOrganism3D*> > > individual_organisms_in_current_year;
        LOG(DEBUG) << "Load environments of year "<<year;
        boost::unordered_map<string, ISEA3H*> current_environments = getEnvironmenMap(year);

        char line[100];

        //Create the active individual organisms via cloning the individual organisms from the previous time step.
        boost::unordered_map<SpeciesObject3D*, boost::unordered_map<int, IndividualOrganism3D*> > actived_individualOrganisms;
        for (auto sp_it : all_individualOrganisms[timeLine[year_i - 1]]) {

            SpeciesObject3D *sp = sp_it.first;
            for (auto c_it : sp_it.second) {
                if (c_it.second.size() > 0) {
                    actived_individualOrganisms[sp][c_it.first] = c_it.second.front();
                }
            }
        }

        int organism_count = 0;

        //Handle the active individual organisms one by one.
        LOG(DEBUG)<<"start to simulate organism by species. Count of species is " << actived_individualOrganisms.size();
        for (auto s_it : actived_individualOrganisms) {
            //If it is the beginning of the simulation, generate a suitable layer for the species;
            if (year == 1) {
                string speciesFolder = "";
                //todo: save to db;
                boost::unordered_map<string, NicheBreadth*> nicheBreadth = s_it.first->getNicheBreadth();
                char tiffName[speciesFolder.length() + 28];
                sprintf(tiffName, "%s/suitable.csv", speciesFolder.c_str());
                boost::unordered_map<string, ISEA3H*> current_environments = getEnvironmenMap(year);
                ISEA3H *map = new ISEA3H();
                LOG(DEBUG)<<"Begin to generate the suitable area";
                auto it = current_environments.begin();
                for (auto item : it->second->getValues()) {
                    int id = item.first;
                    int short v = 0;
                    for (auto item : nicheBreadth) {
                        float env_value = current_environments[item.first]->readByID(id);
                        if ((env_value > item.second->getMax()) || (env_value < item.second->getMin())) {
                            v = 0;
                            break;
                        } else {
                            v = 1;
                        }
                    }
                    if (v == 1) {
                        map->setValue(id, v);
                    }
                }
                map->save(tiffName);
                LOG(DEBUG)<<"END to generate the suitable area";
                exit(1);
            }
            //LOG(INFO)<<"start to simulate organism by organism. Current species is "<< s_it.first << ". Count of organisms is " << s_it.second.size();

            vector<IndividualOrganism3D*> new_individual_organisms;
            for (auto o_it : s_it.second) {
                IndividualOrganism3D *individualOrganism = o_it.second;
                //if current year no smaller than individual organism's next run year, then move this organism.
                //LOG(INFO)<<"Organism index is "<< individualOrganism->getID()<<". Current year is "<<year<<". Next year is "<<individualOrganism->getNextRunYear();
                if (year >= individualOrganism->getNextRunYear()) {
                    set<int> next_cells;
                    switch (individualOrganism->getDispersalMethod()) {
                    //only the new individual organisms can move
                    case 1:
                        ;
                        break;
                        //all the individual organisms can move
                    case 2:
                        individualOrganism->setRandomDispersalAbility();
                        //LOG(INFO)<<"CALL getDispersalMap_2";
                        next_cells = getDispersalMap_2(individualOrganism, year);
                        break;
                    default:
                        ;
                    }
                    for (auto it : next_cells) {

                        //create a new organism
                        IndividualOrganism3D *new_individualOrganism = new IndividualOrganism3D(year, individualOrganism->getSpecies(), individualOrganism, it);
                        new_individualOrganism->setRandomDispersalAbility();
                        new_individual_organisms.push_back(new_individualOrganism);
                    }

                    next_cells.clear();

                } else {
//                  LOG(INFO) << "Didn't run, for current year is "<<year<< " and organism run year is " << individualOrganism->getNextRunYear();
                }
            }

            for (auto it : new_individual_organisms) {
                int index = it->getID();
                //species id, index
                individual_organisms_in_current_year[s_it.first][index].push_back(it);
            }
            organism_count += new_individual_organisms.size();
            new_individual_organisms.clear();
            //LOG(INFO)<<"end to simulate organism by organism.";
        }

        //LOG(INFO)<<"end to simulate organism by species. Count of species is " << actived_individualOrganisms.size() << ". Count of all organisms is " << organism_count;
        //LOG(INFO)<<"end to simulate cell by cell";

        //remove the unsuitable organisms
        //LOG(INFO)<<"begin to remove the unsuitable organisms.";
        boost::unordered_map<SpeciesObject3D*, vector<int>> erased_keys;

        for (auto s_it : individual_organisms_in_current_year) {
            //LOG(INFO)<<"start to remove unsuitable organisms.";
            vector<int> erased_key;
            for (auto it : s_it.second) {
                if (it.second.size() > 0) {
                    if (!it.second.front()->isSuitable(&current_environments)) {
                        //LOG(INFO)<<"Remove it because of unsuitable";
                        erased_key.push_back(it.first);
                    }
                }
            }
            erased_keys[s_it.first] = erased_key;
            //LOG(INFO)<<"end to remove unsuitable organisms.";
        }
        for (auto sp_it : erased_keys) {
            for (auto key : sp_it.second) {
                for (vector<IndividualOrganism3D*>::iterator it = individual_organisms_in_current_year[sp_it.first][key].begin(); it != individual_organisms_in_current_year[sp_it.first][key].end(); ++it) {
//                    (*it)->getParent()->removeChild(*it);
                    delete *it;
                    organism_count--;
                }
                individual_organisms_in_current_year[sp_it.first][key].clear();
                vector<IndividualOrganism3D*>().swap(individual_organisms_in_current_year[sp_it.first][key]);
                individual_organisms_in_current_year[sp_it.first].erase(key);
            }
        }
        //LOG(INFO)<<"after remove unsuitable, Count of all organisms is " << organism_count;

        boost::unordered_map<SpeciesObject3D*, vector<int>> erased_keys2;
        //Remove the species which distribution is smaller than X for Y time steps
        for (auto sp_it : individual_organisms_in_current_year) {
            SpeciesObject3D *species = sp_it.first;
            /*LOG(INFO)<<"Group map size"<<sp_it.second.size()<<" CurrentSpeciesExtinctionTimeSteps"<<
             species->getCurrentSpeciesExtinctionTimeSteps()<<"/"<<species->getSpeciesExtinctionTimeSteps()<<
             " MaxSpeciesDistribution:"<<species->getMaxSpeciesDistribution()<<
             " SpeciesExtinctionThreaholdPercentage:"<<species->getSpeciesExtinctionThreaholdPercentage();
             */
            if ((sp_it.second.size() > 0) && ((species->getCurrentSpeciesExtinctionTimeSteps() < species->getSpeciesExtinctionTimeSteps())) && (sp_it.second.size() >= (species->getMaxSpeciesDistribution() * species->getSpeciesExtinctionThreaholdPercentage()))) {

                species->setMaxSpeciesDistribution((sp_it.second.size() > species->getMaxSpeciesDistribution()) ? sp_it.second.size() : species->getMaxSpeciesDistribution());
                if ((sp_it.second.size() <= species->getSpeciesExtinctionThreshold()) && (year >= 1000)) {
                    species->addCurrentSpeciesExtinctionTimeSteps();
                } else {
                    species->setCurrentSpeciesExtinctionTimeSteps(0);
                }
                //LOG(INFO)<<"1";
            } else {
                vector<int> erased_key;
                for (auto it : sp_it.second) {
                    if (it.second.size() > 0) {
                        erased_key.push_back(it.first);
                    }
                }
                erased_keys2[sp_it.first] = erased_key;
                //LOG(INFO)<<"2";
            }
        }
        for (auto sp_it : erased_keys2) {
            for (auto key : sp_it.second) {
                for (vector<IndividualOrganism3D*>::iterator it = individual_organisms_in_current_year[sp_it.first][key].begin(); it != individual_organisms_in_current_year[sp_it.first][key].end(); ++it) {
//                    (*it)->getParent()->removeChild(*it);
                    delete *it;
                }
                individual_organisms_in_current_year[sp_it.first][key].clear();
                vector<IndividualOrganism3D*>().swap(individual_organisms_in_current_year[sp_it.first][key]);
                individual_organisms_in_current_year[sp_it.first].erase(key);
            }
            sp_it.second.clear();
            sp_it.first->setDisappearedYear(year);
            individual_organisms_in_current_year.erase(sp_it.first);
        }

        //LOG(INFO)<<"end to remove unsuitable organisms.";

        //mark the group id for every organisms in this year, seperated by species id;
        //LOG(INFO)<<"Begin to mark the group id, and detect the speciation.";
        for (auto sp_it : individual_organisms_in_current_year) {
            //printf("Species ID:%u\n", sp_it.first->getID());
            boost::unordered_map<int, vector<IndividualOrganism3D*> > organisms = sp_it.second;
            SpeciesObject3D *species = sp_it.first;
            int short current_group_id = 1;
            if (year >= (species->getBurnInYear() + species->getSpeciationYears())) {
                //if (year>=(burnInYear + species->getSpeciationYears())) {
                //LOG(INFO)<<"Begin to mark the organism.";
                IndividualOrganism3D *unmarked_organism = getUnmarkedOrganism(&organisms);
                //LOG(INFO)<<"111";
                while (unmarked_organism != NULL) {
                    //LOG(INFO)<<"Unmarked organism is "<<unmarked_organism->getX() <<", "<<unmarked_organism->getY()
                    //      <<" dispersal ability is "<<unmarked_organism->getDispersalAbility()<<". current_group_id is "<<current_group_id;
                    markJointOrganism(current_group_id, unmarked_organism, &organisms);
                    current_group_id++;
                    unmarked_organism = getUnmarkedOrganism(&organisms);
                }

                /*if (current_group_id>1000){
                 LOG(INFO)<<"current_group_id is "<<current_group_id;
                 exit(1);
                 }*/
                //LOG(INFO)<<"End to mark the organism.";
                //detect the speciation
                int short temp_species_id = 1;
//              vector<string> group_output;
                //LOG(INFO)<<"Begin to detect speciation.";
                for (int short group_id_1 = 1; group_id_1 < current_group_id - 1; group_id_1++) {
                    //LOG(INFO)<<"getTempSpeciesID 1 for group "<<group_id_1 <<" current_group_id is "<<current_group_id;
                    int short temp_species_id_1 = getTempSpeciesID(group_id_1, &organisms);
                    for (int short group_id_2 = group_id_1 + 1; group_id_2 < current_group_id; group_id_2++) {
                        //LOG(INFO)<<"getTempSpeciesID 2 for group "<<group_id_2 <<" current_group_id is "<<current_group_id;
                        int short temp_species_id_2 = getTempSpeciesID(group_id_2, &organisms);
                        //if both groups were marked, and they have the same id, skip it.
                        if ((temp_species_id_1 != 0) && (temp_species_id_2 != 0) && (temp_species_id_1 == temp_species_id_2)) {
//                          char line[100];
//                          sprintf(line, "%u,%u,%u,%u,%u,%u,%s", year, group_id_1, group_id_2, temp_species_id_1, temp_species_id_2, 0, "skip");
//                          group_output.push_back(line);
                            continue;
                        }
                        //LOG(INFO)<<"get min_divided_year.";
                        int min_divided_year = getMinDividedYear(sp_it.first->getSpeciationYears(), group_id_1, group_id_2, &organisms, year);
                        //LOG(INFO)<<"get min_divided_year is "<<min_divided_year;

                        if (min_divided_year >= species->getSpeciationYears()) {
                            //LOG(INFO)<<"mark species id with new id.";
                            //if a speciation happened, marked them with two ids if they were not marked.
                            if (temp_species_id_1 == 0) {
//                              char line[100];
//                              sprintf(line, "%u,%u,%u,%u,%u,%u,%s", year, group_id_1, group_id_2, temp_species_id, 0, min_divided_year, "new species, mark group 1");
//                              group_output.push_back(line);
                                markedSpeciesID(group_id_1, temp_species_id, &organisms);
                                temp_species_id_1 = temp_species_id;
                                temp_species_id++;
                            }
                            if (temp_species_id_2 == 0) {
//                              char line[100];
//                              sprintf(line, "%u,%u,%u,%u,%u,%u,%s", year, group_id_1, group_id_2, temp_species_id, 0, min_divided_year, "new species, mark group 2");
//                              group_output.push_back(line);
                                markedSpeciesID(group_id_2, temp_species_id, &organisms);
                                temp_species_id_2 = temp_species_id;
                                temp_species_id++;
                            }
                            //LOG(INFO)<<"end mark species id with new id.";
                        } else {
                            //LOG(INFO)<<"mark species id with old id.";
                            //if there is not speciation, marked them with the same id
                            int short t_id = (temp_species_id_1 == 0) ? temp_species_id_2 : temp_species_id_1;
                            t_id = (t_id == 0) ? temp_species_id : t_id;
                            markedSpeciesID(group_id_1, t_id, &organisms);
                            temp_species_id_1 = t_id;
//                            char line1[100];
//                            sprintf(line1, "%u,%u,%u,%u,%u,%u,%s", year, group_id_1, group_id_2, t_id, 0, min_divided_year, "same species, mark group 1");
//                            group_output.push_back(line1);
                            markedSpeciesID(group_id_2, t_id, &organisms);
                            temp_species_id_2 = t_id;
                            //LOG(INFO)<<"end to mark species id with old id.";
//                            char line2[100];
//                            sprintf(line2, "%u,%u,%u,%u,%u,%u,%s", year, group_id_1, group_id_2, t_id, 0, min_divided_year, "same species, mark group 2");
//                            group_output.push_back(line2);
                        }
                    }
                }
                //LOG(INFO)<<"End to detect speciation.";

            } else {
                for (auto y_it : sp_it.second) {
                    for (auto o_it : y_it.second) {
                        o_it->setGroupId(current_group_id);
                    }
                }
            }
        }
        //LOG(INFO)<<"end to mark the group id, and detect the speciation.";

        //LOG(INFO)<<"Begin to rebuild the organism structure in this year";
        boost::unordered_map<SpeciesObject3D*, boost::unordered_map<int, vector<IndividualOrganism3D*> > > new_individual_organisms_in_current_year;
        for (auto sp_it : individual_organisms_in_current_year) {
            boost::unordered_map<int, vector<IndividualOrganism3D*> > organisms = sp_it.second;
            //count all the species
            boost::unordered_map<int short, int short> species_ids;
            boost::unordered_set<int short> temp_species_ids;
            for (auto c_it : sp_it.second) {
                if (c_it.second.size() > 0) {
                    IndividualOrganism3D *organism_item = c_it.second.front();
                    temp_species_ids.insert(organism_item->getTempSpeciesId());
                }
            }
            int short i = 1;
            for (auto it : temp_species_ids) {
                species_ids[it] = i++;
            }

            if (species_ids.size() > 1) {
                for (auto sp_id_it : species_ids) {
                    SpeciesObject3D *new_species = new SpeciesObject3D(sp_id_it.second, sp_it.first, year);
                    //createSpeciesFolder(new_species, false);
                    species.push_back(new_species);
                    for (auto c_it : sp_it.second) {
                        for (auto o_it : c_it.second) {
                            if (o_it->getTempSpeciesId() == sp_id_it.first) {
                                new_individual_organisms_in_current_year[new_species][c_it.first].push_back(o_it);
                                o_it->setSpecies(new_species);
                            }
                        }
                    }

                }
            } else {
                new_individual_organisms_in_current_year[sp_it.first] = sp_it.second;
            }
        }
        individual_organisms_in_current_year = new_individual_organisms_in_current_year;

        //LOG(INFO)<<"End to rebuild the organism structure in this year";
        //LOG(INFO)<<"begin to generate group maps";
        boost::unordered_map<SpeciesObject3D*, ISEA3H*> group_maps;
        for (auto sp_it : individual_organisms_in_current_year) {
            SpeciesObject3D *species = sp_it.first;
            if (group_maps.find(species) == group_maps.end()) {
                group_maps[sp_it.first] = new ISEA3H();
            }
            if (sp_it.second.size() > 0) {
                for (auto o_id : sp_it.second) {
                    if (o_id.second.size() > 0) {
                        group_maps[sp_it.first]->setValue(o_id.second[0]->getID(), o_id.second[0]->getGroupId());
                    }
                }
            } else {
                //sp_it.first->setDisappearedYear(year);
                group_maps[sp_it.first] = NULL;
            }

        }

        saveGroupmap(year, group_maps);

        all_individualOrganisms.insert(make_pair(year, individual_organisms_in_current_year));

        //remove the useless organism
        //LOG(INFO)<<"Remove the useless organisms. Before removing, Memory usage:"<<CommonFun::getCurrentRSS();
        for (auto sp_it : species) {
            if (year < sp_it->getDispersalSpeed()) {
                continue;
            }
            int speciation_year = sp_it->getSpeciationYears();
            bool is_remove_previous_span = false;
            bool is_remove_previous_speciation = false;
            //todo: double check the logic here.
            if (year < (sp_it->getBurnInYear() - sp_it->getSpeciationYears())) {
                is_remove_previous_span = true;
            }
            if (year > sp_it->getBurnInYear()) {
                is_remove_previous_speciation = true;
            }
            int removed_year = -1;
            if (is_remove_previous_span) {
                removed_year = year - sp_it->getDispersalSpeed();
            }
            if (is_remove_previous_speciation) {
                removed_year = (year - sp_it->getDispersalSpeed()) - speciation_year;
            }
            if (removed_year >= 0) {
                //LOG(INFO)<<"Current year is "<<year<<". Remove organisms at year "<< removed_year<<".";
                boost::unordered_map<int, vector<IndividualOrganism3D*> > temp_o = all_individualOrganisms[removed_year][sp_it];
                for (auto it1 : temp_o) {
                    CommonFun::clearVector(&it1.second);
                }
//              all_individualOrganisms.erase(removed_year);

//              LOG(INFO)<<"Set the parent to NULL for year "<<removed_year + sp_it->getDispersalSpeed();
                temp_o = all_individualOrganisms[removed_year + sp_it->getDispersalSpeed()][sp_it];
                for (auto it1 : temp_o) {
                    for (auto it2 : it1.second) {
                        it2->setParent(NULL);
                    }
                }

//              LOG(INFO)<<"Done!";

            }
        }
        //LOG(INFO)<<"Remove the useless organisms. After  removing, Memory usage:"<<CommonFun::getCurrentRSS();

        //LOG(INFO)<<"Generate speciation information.";
        generateSpeciationInfo(year, false);
        //LOG(INFO)<<"Save stat information.";
        int long o_size = 0;
        int long c_size = 0;
        int long mem_size = 0;
        int long species_size = 0;

        sprintf(line, "%u,%lu,%lu,%lu,%lu,%lu,%lu", year, CommonFun::getCurrentRSS(), c_size, o_size, mem_size, species_size, all_individualOrganisms[year].size());
        stat_output.push_back(line);

        if ((CommonFun::getCurrentRSS() > memLimit)) {
            return 1;
        }
    }

    generateSpeciationInfo(this->total_years, true);
    return 0;
}
void Simulation3D::generateSpeciationInfo(int year, bool is_tree) {
    vector<SpeciesObject3D*> roots;
    for (auto sp_it : species) {
        if (sp_it->getAppearedYear() == 0) {
            sp_it->markNode(year);
            roots.push_back(sp_it);
        }
    }

    for (auto sp_it : roots) {
        string folder = this->targetFolder;
        if (is_tree) {
            string newick = folder + "/tree.new";
            CommonFun::writeFile(sp_it->getNewickTree(true, false, year),
                    newick.c_str());
            string html = folder + "/Phylogram.html";
            CommonFun::writeFile(sp_it->getHTMLTree(year), html.c_str());
        }

        string stat = folder + "/stats/stat.csv";
        CommonFun::writeFile(sp_it->getSpeciationExtinction(true, year),
                stat.c_str());

    }
}
int short Simulation3D::getTempSpeciesID(int short group_id, boost::unordered_map<int, vector<IndividualOrganism3D*> > *organisms) {
    for (auto c_it : (*organisms)) {
        for (auto o_it : c_it.second) {
            if (o_it->getGroupId() == group_id) {
                return o_it->getTempSpeciesId();
            }
        }
    }
    return 0;
}
void Simulation3D::markedSpeciesID(int short group_id,
        int short temp_species_id,
        boost::unordered_map<int, vector<IndividualOrganism3D*> > *organisms) {
    /// @todo Here can improve the efficiency a lot! We need a new algorithm!
    //LOG(INFO)<<"Size of organism 1 is "<<organisms->size();
    for (auto c_it : (*organisms)) {
        //LOG(INFO)<<"Size of organism 2 is "<<c_it.second.size();
        for (auto o_it : c_it.second) {
            if (o_it->getGroupId() == group_id) {
                o_it->setTempSpeciesId(temp_species_id);
            }
        }
    }
}
int Simulation3D::distance3D(int id1, int id2, int limited) {

    return neighborInfo->distance(id1, id2, limited);
}

int Simulation3D::getMinDividedYear(int speciation_year, int short group_id_1, int short group_id_2, boost::unordered_map<int, vector<IndividualOrganism3D*> > *organisms, int current_year) {
    int nearest_divided_year = 0;
    vector<int> group_c_1;
    vector<int> group_c_2;
    int group_1_index = 0;
    int group_2_index = 0;

    //save all the organisms in that two groups
    vector<IndividualOrganism3D*> group_1 = (*organisms)[group_1_index];
    vector<IndividualOrganism3D*> group_2 = (*organisms)[group_2_index];

    for (auto c_it : (*organisms)) {
        for (auto o_it : c_it.second) {
            if (o_it->getGroupId() == group_id_1) {
                group_1.push_back(o_it);
            } else if (o_it->getGroupId() == group_id_2) {
                group_2.push_back(o_it);
            }
        }
    }

    for (auto o_it_1 : group_1) {
        for (auto o_it_2 : group_2) {
            int divided_year = getDividedYear(o_it_1, o_it_2);
            nearest_divided_year =
                    (divided_year > nearest_divided_year) ?
                            divided_year : nearest_divided_year;
            if ((current_year - nearest_divided_year) < speciation_year) {
                return current_year - nearest_divided_year;
            }
        }
        //printf("%u/%u\n", i++, group_1.size() * group_2.size());
    }
    return current_year - nearest_divided_year;
}
int Simulation3D::getDividedYear(IndividualOrganism3D *o_1, IndividualOrganism3D *o_2) {
    IndividualOrganism3D *parent_1 = o_1->getParent();
    IndividualOrganism3D *parent_2 = o_2->getParent();
    if ((parent_1 == NULL) || (parent_2 == NULL)) {
        return 0;
    }
    if (parent_1->getGroupId() == parent_2->getGroupId()) {
        return parent_1->getYear();
    } else {
        return getDividedYear(parent_1, parent_2);
    }
}
set<int> Simulation3D::getNeighbors(int id, int distance) {
    set<int> neighbors;
    set<int> handled_ids;
    neighborInfo->getNeighborByID(id, distance, &neighbors, &handled_ids);
    handled_ids.clear();
    return neighbors;
}
void Simulation3D::markJointOrganism(int short p_group_id, IndividualOrganism3D *p_unmarked_organism, boost::unordered_map<int, vector<IndividualOrganism3D*> > *organisms) {

    int short id = p_unmarked_organism->getID();

    int short p_dispersal_ability =
            p_unmarked_organism->getDispersalAbility();
    if (p_dispersal_ability == 0) {
        p_dispersal_ability = 1;
    }
    //LOG(INFO)<<"id="<<id<<" p_dispersal_ability="<<p_dispersal_ability;
    set<int> neighbors = getNeighbors(id, p_dispersal_ability);
    //LOG(INFO)<<"2222";
    for (int n_id : neighbors) {
        // LOG(INFO)<<"Neighbor is "<<n_id;
        if (organisms->find(n_id) == organisms->end()) {
            //LOG(INFO) << "skip 2";
            continue;
        }
        int short group_id = (*organisms)[n_id].front()->getGroupId();
        //LOG(INFO)<<"GROUP ID OF '"<<n_id<<"' IS: "<<group_id;
        if (group_id != 0) {
            //LOG(INFO) << "skip 3";
            continue;
        }
        for (auto it : (*organisms)[n_id]) {
            it->setGroupId(p_group_id);
            if (n_id != id) {
                markJointOrganism(p_group_id, it, organisms);
            }
        }
    }
    neighbors.clear();
}

IndividualOrganism3D* Simulation3D::getUnmarkedOrganism(
        boost::unordered_map<int, vector<IndividualOrganism3D*> > *organisms) {
    for (auto p_it : (*organisms)) {
        for (auto it : p_it.second) {
            if (it->getGroupId() == 0) {
                return it;
            }
        }
    }
    return NULL;
}
boost::unordered_map<string, ISEA3H*> Simulation3D::getEnvironmenMap(int p_year) {
    boost::unordered_map<string, ISEA3H*> result;
    LOG(DEBUG) << "Environments size is " << environments.size();
    for (auto item : environments) {
        result[item.first] = item.second->getValues(p_year);
    }
    return result;
}
void Simulation3D::cleanSpecies() {
    //CommonFun::clearVector(&species);
}
void Simulation3D::cleanActivedIndividualOrganism3Ds() {
//    CommonFun::clearUnordered_map(all_individualOrganisms);

    for (auto y_it : all_individualOrganisms) {
        for (auto s_it : y_it.second) {
            for (auto l_it : s_it.second) {
                CommonFun::clearVector(&l_it.second);
            }
        }
    }
//    CommonFun::clearUnordered_map(&all_individualOrganisms);
//    CommonFun::clearUnordered_map(&actived_individualOrganisms);
}
void Simulation3D::cleanEnvironments() {
//    CommonFun::clearVector(&environments);
//    for (hashmap_multiply::iterator it = environment_maps.begin();
//            it != environment_maps.end();) {
//        CommonFun::clearVector(&it->second);
//        it = environment_maps.erase(it);
//    }
//    environment_maps.clear();
}
set<int> Simulation3D::getDispersalMap_2(IndividualOrganism3D *individualOrganism, int year) {
    set<int> new_cells;

    int short p_dispersal_ability = individualOrganism->getDispersalAbility();
    if (individualOrganism->getNumOfPath() == -1) {
        int id = individualOrganism->getID();
        set<int> neighbors = getNeighbors(id, p_dispersal_ability);
        for (int nei_id : neighbors) {
            new_cells.insert(nei_id);
        }
    }
    return new_cells;
}
Simulation3D::~Simulation3D() {
    delete mask;
    CommonFun::executeSQL("CREATE INDEX idx_year ON map (year)", log_db);
    sqlite3_close(log_db);
//  cleanEnvironments();
//  cleanActivedIndividualOrganism3Ds();
//  cleanSpecies();
}

