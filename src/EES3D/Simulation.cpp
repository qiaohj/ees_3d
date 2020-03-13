/**
 * @file Simulation.cpp
 * @brief Class Simulation. A class to define the features of a virtual scenario in a  simulation, and the virtual species in the scenario.
 * @author Huijie Qiao
 * @version 1.0
 * @date 3/7/2020
 * @details
 * Copyright 2014-2020 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 */
#include "Simulation.h"

Simulation::Simulation(Species *p_species, string label, int burnInYear, string target, bool p_overwrite, unsigned long memLimit,
        vector<int> &p_timeLine, Neighbor* neighborInfo, vector<string> environment_labels, string mask_table) {
    this->species[p_species->getIDWithParentID()] = p_species;
    this->ancestor = p_species;
    this->burnInYear = burnInYear;
    this->overwrite = p_overwrite;
    this->memLimit = memLimit;
    this->label = label;
    this->timeLine = p_timeLine;
    this->indexSimulation = indexSimulation;
    this->totalSimulation = totalSimulation;
    log_db = NULL;
    this->neighborInfo = neighborInfo;
    this->environment_labels = environment_labels;
    this->mask_table = mask_table;
    /**
    for (auto it : neighborInfo->getNeighbors()) {
        set<int> n;
        set<int> h;
        n = this->getNeighbors(it.first, 1);
        if (n.size() != 7) {
            LOG(INFO) << it.first << " has " << n.size() << " neighbors.";
        }
        //n.clear();
        //h.clear();

    }
    exit(1);
    **/
    mask = NULL;
    targetFolder = target + "/" + label;
}
void Simulation::setIndexSimulation(int indexSimulation){
    this->indexSimulation = indexSimulation;
}
void Simulation::setTotalSimulation(int totalSimulation){
    this->totalSimulation = totalSimulation;
}
void Simulation::setNeighbor(Neighbor *neighborInfo){
    this->neighborInfo = neighborInfo;
}

bool Simulation::getOverwrite(){
    return this->overwrite;
}

/*-------------------------
 * Save the population information for a specific time step to a db
 *-----------------------*/
void Simulation::saveGroupmap(int year_i, boost::unordered_map<Species*, ISEA3H*> *species_group_maps) {
    LOG(DEBUG)<<"Save result to db";
    if (species_group_maps->size() == 0) {
        LOG(ERROR)<<"NO MAP, RETURN";
        return;
    }
    //logs.push_back("insert into map (YEAR, ID, group_id, sp_id ) values ");
    int i = 0;
    for (auto sp_it : (*species_group_maps)) {
        Species *sp = sp_it.first;
        ISEA3H *map = sp_it.second;
        if (map) {
            for (auto item : map->getValues()) {
                int id = item.first;
                int group_id = (int) item.second;
                if (group_id >= 0) {
                    string sp_id = sp->getIDWithParentID();
                    char line[sp_id.size() + 100];
                    if (i == 0) {
                        //sprintf(line, " (%u,%u,%u,%s) ", timeLine[year_i], id, group_id, sp_id.c_str());
                    } else {
                        //sprintf(line, " ,(%u,%u,%u,%s) ", timeLine[year_i], id, group_id, sp_id.c_str());
                    }
                    i = 1;
                    sprintf(line, "%u,%u,%u,%s", timeLine[year_i], id, group_id, sp_id.c_str());
                    logs.push_back(line);
                }
            }

        }
    }
    //logs.push_back(";");

    if (i > 0) {

    }else{
        LOG(ERROR)<<"No map found";
    }
}


string Simulation::getTargetFolder(){
    return this->targetFolder;
}
void Simulation::addEnvironment(string environment_label, EnvironmentalISEA3H *env) {
    this->environments[environment_label] = env;
}

void Simulation::createLogDB() {

    this->logFile = targetFolder + "/" + label + ".sqlite";
    CommonFun::deleteFile(logFile.c_str());
    int rc = sqlite3_open(logFile.c_str(), &log_db);
    if (rc) {
        LOG(ERROR) << "Can't open log database: " << sqlite3_errmsg(log_db) << " at " << logFile;
    } else {
        LOG(INFO) << "Create LOG Database at " << logFile << " successfully";
    }
    //char *zErrMsg = 0;
    //Create a table to save the log
    string sql = "CREATE TABLE map(YEAR INT NOT NULL, ID INT NOT NULL, group_id INT NOT NULL, sp_id CHAR(255));";
    CommonFun::executeSQL(sql, log_db, true);
    sql = "CREATE TABLE suitable(ID INT NOT NULL, is_seed INT NOT NULL);";
    CommonFun::executeSQL(sql, log_db, true);
    sql = "CREATE TABLE trees(TYPE CHAR(255) NOT NULL, CONTENT TEXT NOT NULL);";
    CommonFun::executeSQL(sql, log_db, true);

}
void Simulation::commitLog(){

    //logs.push_back("COMMIT;");
    LOG(INFO)<<"Outputting log file";
    string logFile = this->targetFolder + "/" + label + ".log";
    CommonFun::writeFile(logs, logFile.c_str());
    //LOG(INFO)<<"Outputting log db";
    //CommonFun::executeSQL(logs, log_db, true);
    logs.clear();
}
bool Simulation::init(boost::unordered_map<string, EnvironmentalISEA3H*>* environments_base, sqlite3* env_db, boost::unordered_map<string, ISEA3H*>* masks){
    //logs.push_back("BEGIN TRANSACTION;");
    bool isFinished = boost::filesystem::exists(this->targetFolder);

    /*-------------------
     * If the target folder exists and the is_overwrite parameter is false, skip the simulation,
     * or overwrite the existing result with the new simulation.
     -------------------------*/

    if ((isFinished) && (!overwrite)) {
        LOG(INFO) << "Result folder is exist. Maybe executed by another running thread. Skip this simulation!";
        return false;
    }
    LOG(DEBUG) << "Save result to " << targetFolder;

    //Create the necessary folders.
    CommonFun::createFolder(targetFolder.c_str());
    for (string env_label : this->environment_labels) {
        LOG(DEBUG) << "Trying to load environment " << env_label;
        EnvironmentalISEA3H *env = (*environments_base)[env_label];
        if (env == NULL) {
            LOG(DEBUG) << "No environment found, load it from db";
            env = new EnvironmentalISEA3H(env_label, env_db, timeLine);
            (*environments_base)[env_label] = env;
        }
        LOG(DEBUG) << "Finish to load the environment " << env_label;
        addEnvironment(env_label, env);
    }


    LOG(DEBUG) << "Loading mask";

    if ((*masks).find(mask_table) == (*masks).end()) {
        boost::unordered_map<int, boost::unordered_map<int, float>> mask_v = Utility::readEnvInfo(env_db, mask_table, false);
        LOG(DEBUG) << "NEW mask";
        ISEA3H *mask = new ISEA3H(mask_v[0]);
        (*masks)[mask_table] = mask;
        this->mask = mask;

    } else {
        LOG(DEBUG) << "Loaded mask";
       this->mask = (*masks)[mask_table];

    }
    LOG(DEBUG) << "Finished to load mask";

    LOG(DEBUG) << "Init simulation";
    createLogDB();


    //Load the species parameters.

    set<int> seeds = ancestor->getSeeds();
    LOG(DEBUG)<<0;
    boost::unordered_map<int, vector<Organism*> > t_o;
    /* -----------------
     * Create the individual organism(s) based on the seeds in the species' configuration.
     * All the individual organism(s) has(have) the same parameters inherited from the species and the different localities.
     * Now there is only on seed per species.
     * Don't use the function of the multiple species and multiple seeds per species now,
     * because it hasn't be tested strictly.
     *-------------------------*/
    LOG(DEBUG)<<1<<seeds.size();
    for (int seed : seeds) {
        LOG(DEBUG)<<2<<" "<<seed;
        Organism *organism = new Organism(0, ancestor, NULL, seed);
        LOG(DEBUG)<<3;
        boost::unordered_map<int, Organism*> t;
        LOG(DEBUG)<<4;
        t[seed] = organism;
        LOG(DEBUG)<<5;
        t_o[seed].push_back(organism);
        LOG(DEBUG)<<6;

    }
    LOG(DEBUG) << "ADD ROOT SPECIES TO all_organisms WIHT the id is " << ancestor->getIDWithParentID() << " and SIZE is " << t_o.size();
    all_organisms[0][ancestor] = t_o;
    return true;
}

vector<string> Simulation::getEnvironmentLabels() {
    return this->environment_labels;
}
EnvironmentalISEA3H* Simulation::getEnvironment(string environment_label) {
    return this->environments[environment_label];
}
vector<int> Simulation::getTimeLine() {
    return this->timeLine;
}

void Simulation::setMask(ISEA3H* p_mask){
    this->mask = p_mask;
}
ISEA3H* Simulation::getMask(){
    return this->mask;
}
void Simulation::generateSuitable() {
    boost::unordered_map<string, NicheBreadth*> nicheBreadth = ancestor->getNicheBreadth();
    boost::unordered_map<string, ISEA3H*> current_environments = getEnvironmenMap(0);
    set<int> values;
    LOG(DEBUG) << "Begin to generate the suitable area";
    auto it = current_environments.begin();
    for (auto item : it->second->getValues()) {
        int id = item.first;
        int  v = 0;
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
            values.insert(id);
        }
    }

    vector<string> output;
    char line[100];
    // Note: The old version has only 5 columns without lon and lat columns.
    sprintf(line, "BEGIN TRANSACTION; insert into suitable (ID, is_seed) values ");
    output.push_back(line);
    int i = 0;
    for (int item : values) {
        //LOG(DEBUG)<<this->species.front()->getSeeds().count(item.first);
        bool is_seed = ancestor->getSeeds().count(item) > 0;
        char line[50];
        if (i == 0) {
            sprintf(line, " (%u,%u)", item, is_seed);
        } else {
            sprintf(line, ", (%u,%u)", item, is_seed);
        }
        output.push_back(line);
        i++;
    }
    output.push_back("; COMMIT;");
    if (output.size() > 0) {
        CommonFun::executeSQL(output, log_db, false);
        output.clear();
    }
    LOG(DEBUG) << "END to generate the suitable area";
}
/*---------------------------
 * Run a simulation on a scenario with the species in the scenario.
 *---------------------*/
int Simulation::run() {
    clock_t start, end;
    start = clock();

    boost::unordered_map<Species*, ISEA3H*> species_group_maps;

    LOG(DEBUG)<<"Total timeLine is "<<timeLine.size();
    for (unsigned year_i = 1; year_i<timeLine.size(); year_i++) {
        end = clock();
        double time_taken = double(end - start) / double(CLOCKS_PER_SEC);

        LOG(INFO) << "v3.2 Current year:" << timeLine[year_i] << " @ " << this->targetFolder << " ("<<indexSimulation<<"/"<<totalSimulation<<") N_sp:"<<
                all_organisms[year_i - 1].size()<< ". "<<time_taken/60<<" Mins. Memory usage:" << CommonFun::getCurrentRSS(pow(1024, 2)) << "MB.";

        boost::unordered_map<Species*, boost::unordered_map<int, vector<Organism*> > > organisms_in_current_year;
        //LOG(DEBUG) << "Load environments of year " << timeLine[year_i] << " via index " << year_i;
        boost::unordered_map<string, ISEA3H*> current_environments = getEnvironmenMap(year_i);
        //Create the active individual organisms via cloning the individual organisms from the previous time step.
        boost::unordered_map<Species*, boost::unordered_map<int, Organism*> > actived_organisms;
        //LOG(DEBUG) << "Found " << all_organisms[year_i - 1].size() << " species at time " << timeLine[year_i - 1] << ".";
        for (auto sp_it : all_organisms[year_i - 1]) {
            Species *sp = sp_it.first;
            for (auto c_it : sp_it.second) {
                //LOG(DEBUG)<<"Found "<<c_it.second.size()<< " individual from species "<<sp->getIDWithParentID()<<" at pixel "<<c_it.first;
                if (c_it.second.size() > 0) {
                    actived_organisms[sp][c_it.first] = c_it.second.front();
                }
            }
        }
        //If it is the beginning of the simulation, generate a suitable layer for the species;
        //LOG(DEBUG) << "Current year is " << year << " and timeline[1] is " << timeLine[1];
        if (year_i == 1) {
            generateSuitable();
        }

        int organism_count = 0;

        //Handle the active individual organisms one by one.
        LOG(DEBUG)<<"start to simulate organism by species. Count of species is " << actived_organisms.size();
        for (auto s_it : actived_organisms) {
            //LOG(DEBUG)<<"start to simulate organism by organism. Current species is "<< s_it.first->getIDWithParentID() << ". Count of organisms is " << s_it.second.size();
            vector<Organism*> new_organisms;
            for (auto o_it : s_it.second) {
                Organism *organism = o_it.second;
                //if current year no smaller than individual organism's next run year, then move this organism.
                //LOG(DEBUG)<<"Organism index is "<< organism->getID()<<". Current year is "<<year_i<<". Next year is "<<organism->getNextRunYearI();
                if ((int)year_i >= organism->getNextRunYearI()) {
                    set<int> next_cells;
                    switch (organism->getDispersalMethod()) {
                    //only the new individual organisms can move
                    case 1:
                        ;
                        break;
                        //all the individual organisms can move
                    case 2:
                        organism->setRandomDispersalAbility();
                        next_cells = getDispersalMap_2(organism);
                        break;
                    default:
                        ;
                    }
                    for (auto it : next_cells) {

                        //create a new organism
                        Organism *new_organism = new Organism(year_i, organism->getSpecies(), organism, it);
                        new_organism->setRandomDispersalAbility();
                        new_organisms.push_back(new_organism);
                    }

                    next_cells.clear();

                }
            }
            //LOG(DEBUG) << "new_organisms size is "<<new_organisms.size();
            for (auto it : new_organisms) {
                int id = it->getID();
                //species id, index
                organisms_in_current_year[s_it.first][id].push_back(it);
            }
            organism_count += new_organisms.size();
            new_organisms.clear();
            //LOG(DEBUG)<<"end to simulate organism by organism.";
        }

        LOG(DEBUG)<<"end to simulate organism by species. Count of species is " << actived_organisms.size() << ". Count of all organisms is " << organism_count;

        //remove the unsuitable organisms
        LOG(DEBUG)<<"begin to remove the unsuitable organisms.";
        boost::unordered_map<Species*, vector<int>> erased_keys;

        for (auto s_it : organisms_in_current_year) {
            //LOG(DEBUG)<<"start to remove unsuitable organisms.";
            vector<int> erased_key;
            for (auto it : s_it.second) {
                if (it.second.size() > 0) {
                    if (!it.second.front()->isSuitable(&current_environments, mask)) {
                        //LOG(INFO)<<"Remove it because of unsuitable";
                        erased_key.push_back(it.first);
                    }
                }
            }
            erased_keys[s_it.first] = erased_key;
            //LOG(DEBUG)<<"end to remove unsuitable organisms.";
        }
        for (auto sp_it : erased_keys) {
            for (auto key : sp_it.second) {
                for (vector<Organism*>::iterator it = organisms_in_current_year[sp_it.first][key].begin(); it != organisms_in_current_year[sp_it.first][key].end(); ++it) {
                    delete *it;
                    organism_count--;
                }
                organisms_in_current_year[sp_it.first][key].clear();
                vector<Organism*>().swap(organisms_in_current_year[sp_it.first][key]);
                organisms_in_current_year[sp_it.first].erase(key);
            }
        }
        LOG(DEBUG)<<"after remove unsuitable, Count of all organisms is " << organism_count;

        boost::unordered_map<Species*, vector<int>> erased_keys2;
        for (auto sp_it : organisms_in_current_year) {
            Species *species = sp_it.first;
            if ((sp_it.second.size() > 0) && ((species->getCurrentSpeciesExtinctionTimeSteps() < species->getSpeciesExtinctionTimeSteps())) && (sp_it.second.size() >= (species->getMaxSpeciesDistribution() * species->getSpeciesExtinctionThreaholdPercentage()))) {

                species->setMaxSpeciesDistribution((sp_it.second.size() > species->getMaxSpeciesDistribution()) ? sp_it.second.size() : species->getMaxSpeciesDistribution());
                if ((sp_it.second.size() <= species->getSpeciesExtinctionThreshold()) && (year_i >= 10)) {
                    species->addCurrentSpeciesExtinctionTimeSteps();
                } else {
                    species->setCurrentSpeciesExtinctionTimeSteps(0);
                }
            } else {
                vector<int> erased_key;
                for (auto it : sp_it.second) {
                    if (it.second.size() > 0) {
                        erased_key.push_back(it.first);
                    }
                }
                erased_keys2[sp_it.first] = erased_key;
            }
        }
        for (auto sp_it : erased_keys2) {
            for (auto key : sp_it.second) {
                for (vector<Organism*>::iterator it = organisms_in_current_year[sp_it.first][key].begin(); it != organisms_in_current_year[sp_it.first][key].end(); ++it) {
                    delete *it;
                    organism_count--;
                }
                organisms_in_current_year[sp_it.first][key].clear();
                vector<Organism*>().swap(organisms_in_current_year[sp_it.first][key]);
                organisms_in_current_year[sp_it.first].erase(key);
            }
            sp_it.second.clear();
            sp_it.first->setDisappearedYearI(year_i);
            organisms_in_current_year.erase(sp_it.first);
        }
        LOG(DEBUG)<<"end to remove unsuitable organisms. organisms size is "<< organisms_in_current_year.size();

        //mark the group id for every organisms in this year, seperated by species id;
        LOG(DEBUG)<<"Begin to mark the group id, and detect the speciation.";
        for (auto sp_it : organisms_in_current_year) {
            //printf("Species ID:%u\n", sp_it.first->getID());
            boost::unordered_map<int, vector<Organism*> > organisms = sp_it.second;
            Species *species = sp_it.first;
            int  current_group_id = 1;
            //if ((int)year_i >= (species->getBurnInYear() + species->getSpeciationYears())) {
            if (true){
                //LOG(DEBUG)<<"Begin to mark the organism.";
                int unmarked_id = getUnmarkedID(&organisms);

                while (unmarked_id != -1) {
                    //LOG(INFO)<<"Unmarked organism is "<<unmarked_organism->getX() <<", "<<unmarked_organism->getY()
                    //      <<" dispersal ability is "<<unmarked_organism->getDispersalAbility()<<". current_group_id is "<<current_group_id;
                    int dispersal_ability = -1;
                    for (auto it : organisms[unmarked_id]){
                        dispersal_ability = (dispersal_ability>it->getDispersalAbility())?dispersal_ability:it->getDispersalAbility();
                    }
                    markJointOrganism(current_group_id, unmarked_id, dispersal_ability, &organisms);
                    current_group_id++;
                    //LOG(DEBUG)<<"NEW GROUP ADDED";
                    unmarked_id = getUnmarkedID(&organisms);

                }
                //LOG(DEBUG)<<"End to mark the organism. " << current_group_id-1<<" groups were found.";
                //detect the speciation
                int  temp_species_id = 1;
//              vector<string> group_output;
                LOG(DEBUG)<<"Begin to detect speciation.";
                for (int  group_id_1 = 1; group_id_1 < current_group_id - 1; group_id_1++) {
                    LOG(DEBUG)<<"getTempSpeciesID 1 for group "<<group_id_1 <<" current_group_id is "<<current_group_id;
                    int  temp_species_id_1 = getTempSpeciesID(group_id_1, &organisms);
                    for (int  group_id_2 = group_id_1 + 1; group_id_2 < current_group_id; group_id_2++) {
                        LOG(DEBUG)<<"getTempSpeciesID 2 for group "<<group_id_2 <<" current_group_id is "<<current_group_id;
                        int  temp_species_id_2 = getTempSpeciesID(group_id_2, &organisms);
                        //if both groups were marked, and they have the same id, skip it.
                        if ((temp_species_id_1 != 0) && (temp_species_id_2 != 0) && (temp_species_id_1 == temp_species_id_2)) {
                            continue;
                        }
                        LOG(DEBUG)<<"get min_divided_year.";
                        int min_divided_year = getMinDividedYear(sp_it.first->getSpeciationYears(), group_id_1, group_id_2, &organisms, year_i);
                        LOG(DEBUG)<<"min_divided_year is "<<min_divided_year;

                        if (min_divided_year >= species->getSpeciationYears()) {
                            LOG(DEBUG)<<"mark species id with new id.";
                            //if a speciation happened, marked them with two ids if they were not marked.
                            if (temp_species_id_1 == 0) {
                                markedSpeciesID(group_id_1, temp_species_id, &organisms);
                                temp_species_id_1 = temp_species_id;
                                temp_species_id++;
                            }
                            if (temp_species_id_2 == 0) {
                                markedSpeciesID(group_id_2, temp_species_id, &organisms);
                                temp_species_id_2 = temp_species_id;
                                temp_species_id++;
                            }
                            LOG(DEBUG)<<"end mark species id with new id.";
                        } else {
                            LOG(DEBUG)<<"mark species id with old id.";
                            //if there is not speciation, marked them with the same id
                            int  t_id = (temp_species_id_1 == 0) ? temp_species_id_2 : temp_species_id_1;
                            t_id = (t_id == 0) ? temp_species_id : t_id;
                            markedSpeciesID(group_id_1, t_id, &organisms);
                            temp_species_id_1 = t_id;
                            markedSpeciesID(group_id_2, t_id, &organisms);
                            temp_species_id_2 = t_id;
                            LOG(DEBUG)<<"end to mark species id with old id.";
                        }
                    }
                }
                LOG(DEBUG)<<"End to detect speciation.";

            } else {
                for (auto y_it : sp_it.second) {
                    for (auto o_it : y_it.second) {
                        o_it->setGroupId(current_group_id);
                    }
                }
            }
        }
        LOG(DEBUG)<<"end to mark the group id, and detect the speciation. organisms size is "<<organisms_in_current_year.size();

        LOG(DEBUG)<<"Begin to rebuild the organism structure in this year";
        boost::unordered_map<Species*, boost::unordered_map<int, vector<Organism*> > > new_organisms_in_current_year;
        for (auto sp_it : organisms_in_current_year) {
            boost::unordered_map<int, vector<Organism*> > organisms = sp_it.second;
            //count all the species
            boost::unordered_map<int , int > species_ids;
            boost::unordered_set<int > temp_species_ids;
            for (auto c_it : sp_it.second) {
                if (c_it.second.size() > 0) {
                    Organism *organism_item = c_it.second.front();
                    temp_species_ids.insert(organism_item->getTempSpeciesId());
                }
            }
            int  i = 1;
            for (auto it : temp_species_ids) {
                species_ids[it] = i++;
            }

            if (species_ids.size() > 1) {
                for (auto sp_id_it : species_ids) {
                    Species *new_species = new Species(sp_id_it.second, sp_it.first, year_i);
                    //createSpeciesFolder(new_species, false);
                    species[new_species->getIDWithParentID()] = new_species;
                    for (auto c_it : sp_it.second) {
                        for (auto o_it : c_it.second) {
                            if (o_it->getTempSpeciesId() == sp_id_it.first) {
                                new_organisms_in_current_year[new_species][c_it.first].push_back(o_it);
                                o_it->setSpecies(new_species);
                            }
                        }
                    }

                }
            } else {
                new_organisms_in_current_year[sp_it.first] = sp_it.second;
            }
        }
        organisms_in_current_year = new_organisms_in_current_year;

        LOG(DEBUG)<<"End to rebuild the organism structure in this year. organisms size is "<<organisms_in_current_year.size();
        LOG(DEBUG)<<"begin to generate group maps";
        boost::unordered_map<Species*, ISEA3H*> group_maps;
        for (auto sp_it : organisms_in_current_year) {
            Species *species = sp_it.first;
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

        saveGroupmap(year_i, &group_maps);
        for (auto it : group_maps){
            if (it.second){
                delete it.second;
            }
        }

        all_organisms.insert(make_pair(year_i, organisms_in_current_year));

        //remove the useless organism
        LOG(DEBUG)<<"Remove the useless organisms. Before removing, Memory usage:"<<CommonFun::getCurrentRSS(1);
        for (auto it : species) {
            Species *sp_it = it.second;
            if ((int)year_i < sp_it->getDispersalSpeed()) {
                continue;
            }
            int speciation_year = sp_it->getSpeciationYears();
            bool is_remove_previous_span = false;
            bool is_remove_previous_speciation = false;
            if ((int)year_i < (sp_it->getBurnInYear() - sp_it->getSpeciationYears())) {
                is_remove_previous_span = true;
            }
            if ((int)year_i > sp_it->getBurnInYear()) {
                is_remove_previous_speciation = true;
            }
            int removed_year_i = -1;
            if (is_remove_previous_span) {
                removed_year_i = (int)year_i - sp_it->getDispersalSpeed();
            }
            if (is_remove_previous_speciation) {
                removed_year_i = (int)year_i - sp_it->getDispersalSpeed() - speciation_year;
            }
            if (removed_year_i >= 0) {

                //LOG(INFO)<<"Current year is "<<year<<". Remove organisms at year "<< removed_year<<".";
                boost::unordered_map<int, vector<Organism*> > temp_o = all_organisms[removed_year_i][sp_it];
                for (auto it1 : temp_o) {
                    for (auto it2 : it1.second){
                        delete it2;
                    }
                }
                all_organisms.erase(removed_year_i);

//              LOG(INFO)<<"Set the parent to NULL for year "<<removed_year + sp_it->getDispersalSpeed();
                temp_o = all_organisms[removed_year_i + sp_it->getDispersalSpeed()][sp_it];
                for (auto it1 : temp_o) {
                    for (auto it2 : it1.second) {
                        it2->setParent(NULL);
                    }
                }
//              LOG(INFO)<<"Done!";
            }
        }
        LOG(DEBUG)<<"Remove the useless organisms. After  removing, Memory usage:"<<CommonFun::getCurrentRSS(1);

        if ((CommonFun::getCurrentRSS(pow(1024, 3)) > memLimit)) {
            LOG(INFO) << "Current memory is " << CommonFun::getCurrentRSS(pow(1024, 3)) << "GB. Memory limit is " << memLimit << "GB";
            return 1;
        }
    }

    generateSpeciationInfo(timeLine.size() - 1);

    //CommonFun::executeSQL("CREATE INDEX idx_year ON map (year)", log_db);
    return 0;
}
void Simulation::generateSpeciationInfo(int year_i) {
    vector<Species*> roots;
    for (auto it : species) {
        Species *sp_it = it.second;
        if (sp_it->getAppearedYearI() == 0) {
            sp_it->markNode(year_i);
            roots.push_back(sp_it);
        }
    }

    for (auto sp_it : roots) {
        string tree = sp_it->getNewickTree(true, false, year_i);
        string sql = "INSERT INTO trees (TYPE, CONTENT) VALUES ('NEWICK', " + CommonFun::quoteSql(tree) + ");";
        CommonFun::executeSQL(sql, log_db, true);

        tree = boost::algorithm::join(sp_it->getHTMLTree(year_i), " ");
        sql = "INSERT INTO trees (TYPE, CONTENT) VALUES ('HTML', " + CommonFun::quoteSql(tree) + ");";
        CommonFun::executeSQL(sql, log_db, true);

    }
}
int  Simulation::getTempSpeciesID(int group_id, boost::unordered_map<int, vector<Organism*> > *organisms) {
    for (auto c_it : (*organisms)) {
        for (auto o_it : c_it.second) {
            if (o_it->getGroupId() == group_id) {
                return o_it->getTempSpeciesId();
            }
        }
    }
    return 0;
}
void Simulation::markedSpeciesID(int  group_id,
        int  temp_species_id,
        boost::unordered_map<int, vector<Organism*> > *organisms) {
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
int Simulation::distance(int id1, int id2, int limited) {

    return neighborInfo->distance(id1, id2, limited);
}

int Simulation::getMinDividedYear(int speciation_year, int  group_id_1, int  group_id_2,
        boost::unordered_map<int, vector<Organism*> > *organisms, int current_year_i) {
    int nearest_divided_year_i = 0;
    vector<int> group_c_1;
    vector<int> group_c_2;
    int group_1_index = 0;
    int group_2_index = 0;
    /// @todo improve here
    //save all the organisms in that two groups
    vector<Organism*> group_1 = (*organisms)[group_1_index];
    vector<Organism*> group_2 = (*organisms)[group_2_index];
    //LOG(DEBUG)<<"1";
    for (auto c_it : (*organisms)) {
        //LOG(DEBUG)<<"5";
        for (auto o_it : c_it.second) {
            //LOG(DEBUG)<<"6";
            if (o_it->getGroupId() == group_id_1) {
                group_1.push_back(o_it);
            } else if (o_it->getGroupId() == group_id_2) {
                group_2.push_back(o_it);
            }
        }
    }
    //LOG(DEBUG)<<"2";
    for (auto o_it_1 : group_1) {
        //LOG(DEBUG)<<"7";
        for (auto o_it_2 : group_2) {
            //LOG(DEBUG)<<"8";
            int divided_year_i = getDividedYearI(o_it_1, o_it_2);
            nearest_divided_year_i = (divided_year_i > nearest_divided_year_i) ? divided_year_i : nearest_divided_year_i;
            if (nearest_divided_year_i < speciation_year) {
                return current_year_i - nearest_divided_year_i;
            }
        }
        //printf("%u/%u\n", i++, group_1.size() * group_2.size());
    }
    return current_year_i - nearest_divided_year_i;
}
int Simulation::getDividedYearI(Organism *o_1, Organism *o_2) {
    Organism *parent_1 = o_1->getParent();
    Organism *parent_2 = o_2->getParent();
    if ((parent_1 == NULL) || (parent_2 == NULL)) {
        return 0;
    }
    if (parent_1->getGroupId() == parent_2->getGroupId()) {
        return parent_1->getYearI();
    } else {
        return getDividedYearI(parent_1, parent_2);
    }
}
set<int> Simulation::getNeighbors(int id, int distance) {
    set<int> neighbors;
    set<int> handled_ids;
    neighborInfo->getNeighborByID(id, distance, &neighbors, &handled_ids);
    handled_ids.clear();
    return neighbors;
}
void Simulation::markJointOrganism(int p_group_id, int unmarked_id, int dispersal_ability, boost::unordered_map<int, vector<Organism*> > *organisms) {

    if (dispersal_ability == 0) {
        dispersal_ability = 1;
    }
    //LOG(DEBUG)<<"id="<<id<<" p_dispersal_ability="<<p_dispersal_ability;
    set<int> neighbors = getNeighbors(unmarked_id, dispersal_ability);
    //string n_ids = "";
    //string n_ids_x = "";
    //string n_ids_y = "";
    for (int n_id : neighbors) {
        //n_ids += "," + to_string(n_id);
        if (organisms->find(n_id) == organisms->end()) {
            continue;
        }
        //n_ids_x += "," + to_string(n_id);
        vector<Organism*> organisms_in_id = (*organisms)[n_id];
        int group_id = organisms_in_id.front()->getGroupId();
        //LOG(DEBUG)<<"GROUP ID OF '"<<n_id<<"' IS: "<<group_id;
        if (group_id != 0) {
            continue;
        }
        //n_ids_y += "," + to_string(n_id);
        dispersal_ability = -1;
        for (auto it : organisms_in_id) {
            it->setGroupId(p_group_id);
            dispersal_ability = (dispersal_ability>it->getDispersalAbility())?dispersal_ability:it->getDispersalAbility();
        }
        if (n_id != unmarked_id) {
            markJointOrganism(p_group_id, n_id, dispersal_ability, organisms);
        }
    }
    //LOG(DEBUG)<<"all neighbor      :"<<unmarked_id<<n_ids;
    //LOG(DEBUG)<<"no data removed   :"<<unmarked_id<<n_ids_x;
    //LOG(DEBUG)<<"grouped id removed:"<<unmarked_id<<n_ids_y;
    //LOG(DEBUG)<<"End to marked a group, which is "<<p_group_id;
    neighbors.clear();
}

int Simulation::getUnmarkedID(boost::unordered_map<int, vector<Organism*>> *organisms) {
    for (auto p_it : (*organisms)) {
        if (p_it.second.front()->getGroupId()==0){
            return p_it.first;
        }
    }
    return -1;
}
boost::unordered_map<string, ISEA3H*> Simulation::getEnvironmenMap(int p_year) {
    boost::unordered_map<string, ISEA3H*> result;
    //LOG(DEBUG) << "Environments size is " << environments.size();
    for (auto item : environments) {
        result[item.first] = item.second->getValues(p_year);
    }
    return result;
}


set<int> Simulation::getDispersalMap_2(Organism *organism) {
    set<int> new_cells;
    int p_dispersal_ability = organism->getDispersalAbility();
    if (organism->getNumOfPath() == -1) {
        int id = organism->getID();
        //LOG(DEBUG) << "Looking for neighbors for " << id << ", which dispersal ability is " << p_dispersal_ability << ".";
        new_cells = getNeighbors(id, p_dispersal_ability);
        //LOG(DEBUG)<<new_cells.size() <<" neighbors were found.";
    }
    return new_cells;
}
Simulation::~Simulation() {

    LOG(DEBUG)<<"MEMORY USAGE BEFORE RELEASE: "<<CommonFun::getCurrentRSS(1);
    sqlite3_close(log_db);
    //boost::unordered_map<int, boost::unordered_map<Species*, boost::unordered_map<int, vector<Organism*> > > >
    LOG(DEBUG)<<1;
    for (auto it1 : all_organisms){
        LOG(DEBUG)<<2;
        for (auto it2 : it1.second){
            LOG(DEBUG)<<3;
            for (auto it3 : it2.second){
                LOG(DEBUG)<<4;
                for (unsigned i = 0; i < it3.second.size(); i++) {
                    LOG(DEBUG)<<5;
                    if (it3.second[i]){
                        LOG(DEBUG)<<6;
                        delete it3.second[i];
                    }
                }
                LOG(DEBUG)<<6;
            }
            LOG(DEBUG)<<7;
        }
    }
    LOG(DEBUG)<<8;
    for (auto it : species){
        LOG(DEBUG)<<9;
        if (it.second){
            delete it.second;
        }
    }
    LOG(DEBUG)<<"MEMORY USAGE AFTER RELEASE: "<<CommonFun::getCurrentRSS(1);

    //delete mask;

//  cleanEnvironments();
//  cleanActivedOrganisms();
//  cleanSpecies();
}


