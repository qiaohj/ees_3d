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

#include <utility>
Simulation::Simulation(Species *p_species, string label, int burnInYear, string target, bool p_overwrite, unsigned long memLimit,
        vector<int> *p_timeLine, Neighbor* neighborInfo, vector<string> *environment_labels, string mask_table) {
    this->t1 = 0;
    this->t2 = 0;
    this->all_species[p_species->getIDWithParentID()] = p_species;
    this->ancestor = p_species;
    this->burnInYear = burnInYear;
    this->overwrite = p_overwrite;
    this->memLimit = memLimit;
    this->label = label;
    this->timeLine = *p_timeLine;
    this->indexSimulation = indexSimulation;
    this->totalSimulation = totalSimulation;
    this->log_db = NULL;
    this->neighborInfo = neighborInfo;
    this->environment_labels = *environment_labels;
    this->mask_table = mask_table;
    this->mask = NULL;
    this->targetFolder = target + "/" + label;
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
void Simulation::saveGroupmap(int year_i, unordered_map<Species*, ISEA*> &species_group_maps) {
    LOG(DEBUG)<<"Save result to db";
    if (species_group_maps.size() == 0) {
        LOG(ERROR)<<"NO MAP, RETURN";
        return;
    }
    //logs.push_back("insert into map (YEAR, ID, group_id, sp_id ) values ");
    int i = 0;
    for (auto sp_it : species_group_maps) {
        Species *sp = sp_it.first;
        ISEA *map = sp_it.second;
        if (map) {
            unordered_map<int, float> values = map->getValues();
            for (auto item : values) {
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
void Simulation::addEnvironment(string environment_label, EnvVar *env) {
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
}
bool Simulation::init(unordered_map<string, EnvVar*> &environments_base, sqlite3* env_db, unordered_map<string, ISEA*> &masks){
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
    for (string env_label : environment_labels) {
        LOG(DEBUG) << "Trying to load environment " << env_label;
        EnvVar *env = environments_base[env_label];
        if (env == NULL) {
            LOG(DEBUG) << "No environment found, load it from db for "<<env_label;
            env = new EnvVar(env_label, env_db);
            environments_base[env_label] = env;
        }
        LOG(DEBUG) << "Finish to load the environment " << env_label;
        addEnvironment(env_label, env);
    }


    LOG(DEBUG) << "Loading mask";

    if (masks.find(mask_table) == masks.end()) {
        unordered_map<int, ISEA*> mask_v;
        Utility::readEnvInfo(env_db, mask_table, false, mask_v);
        masks[mask_table] = mask_v[0];
    }
    this->mask = masks[mask_table];
    LOG(DEBUG) << "Finished to load mask";

    LOG(DEBUG) << "Init simulation";
    createLogDB();


    //Load the species parameters.

    set<int> seeds = ancestor->getSeeds();
    /* -----------------
     * Create the individual organism(s) based on the seeds in the species' configuration.
     * All the individual organism(s) has(have) the same parameters inherited from the species and the different localities.
     * Now there is only on seed per species.
     * Don't use the function of the multiple species and multiple seeds per species now,
     * because it hasn't be tested strictly.
     *-------------------------*/
    vector<Organism*> orgamisms;
    for (int seed : seeds) {
        Organism *organism = new Organism(0, ancestor, NULL, seed);
        orgamisms.push_back(organism);
    }

    all_organisms[0] = orgamisms;

    return true;
}

vector<string> Simulation::getEnvironmentLabels() {
    return environment_labels;
}
EnvVar* Simulation::getEnvironment(string environment_label) {
    return this->environments[environment_label];
}
vector<int> Simulation::getTimeLine() {
    return timeLine;
}

void Simulation::setMask(ISEA* p_mask){
    this->mask = p_mask;
}
ISEA* Simulation::getMask(){
    return this->mask;
}
void Simulation::generateSuitable() {
    unordered_map<string, NicheBreadth*> nicheBreadth = ancestor->getNicheBreadth();
    unordered_map<string, ISEA*> current_environments = getEnvironmentMap(timeLine.front());
    set<int> values;
    LOG(DEBUG) << "Begin to generate the suitable area";
    auto it = current_environments.begin();
    unordered_map<int, float> env_v = it->second->getValues();
    for (auto item : env_v) {
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

    }
    LOG(DEBUG) << "END to generate the suitable area";
}
/*---------------------------
 * Run a simulation on a scenario with the species in the scenario.
 *---------------------*/
int Simulation::run() {
    clock_t start, end;
    start = clock();

    LOG(DEBUG)<<"Total timeLine is "<<timeLine.size();
    unordered_map<Species*, unordered_map<int, vector<Organism*> > > organisms_in_current_year;
    LOG(DEBUG)<<"Add ancestor as the first dataset";
    unordered_map<int, vector<Organism*>> seeds;
    for (auto it : all_organisms[0]){
        seeds[it->getID()].push_back(it);
    }
    organisms_in_current_year[this->ancestor] = seeds;
    for (unsigned year_i = 1; year_i<timeLine.size(); year_i++) {
        end = clock();
        double time_taken = double(end - start) / double(CLOCKS_PER_SEC);

        LOG(INFO) << "v3.7 Current year:" << timeLine[year_i] << " @ " << this->targetFolder << " ("<<indexSimulation<<"/"<<totalSimulation<<") N_sp:"<<
                //organisms_in_current_year->size()<< ". "<<time_taken/60<<" Mins. Memory usage:" << CommonFun::getCurrentRSS(pow(1024, 2)) << "MB.";
                organisms_in_current_year.size()<< ". "<<time_taken/60<<" Mins. Memory usage:" << CommonFun::getCurrentRSS(pow(1024, 2)) << "MB.";
        LOG(DEBUG) << "Load environments of year " << timeLine[year_i] << " via index " << year_i;

        unordered_map<string, ISEA*> current_environments = getEnvironmentMap(timeLine[year_i]);
        //Create the active individual organisms via cloning the individual organisms from the previous time step.
        unordered_map<Species*, unordered_map<int, Organism*>> actived_organisms;
        LOG(DEBUG) << "Found " << all_organisms[year_i - 1].size() << " organisms at time " << timeLine[year_i - 1] << ".";
        for (auto sp_it : organisms_in_current_year) {
            Species *sp = sp_it.first;
            for (auto c_it : sp_it.second) {
                //LOG(DEBUG)<<"Found "<<c_it.second.size()<< " individual from species "<<sp->getIDWithParentID()<<" at pixel "<<c_it.first;
                if (c_it.second.size() > 0) {
                    actived_organisms[sp][c_it.first] = c_it.second.front();
                }
            }
        }
        //clear organisms_in_current_year
        for (auto it : organisms_in_current_year){
            for (auto it2 : it.second){
                it2.second.clear();
            }
            it.second.clear();
        }
        organisms_in_current_year.clear();
        //If it is the beginning of the simulation, generate a suitable layer for the species;
        LOG(DEBUG) << "Current year is " << year_i << " and timeline[1] is " << timeLine[year_i];
        if (year_i == 1) {
            generateSuitable();
        }

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
                        getDispersalMap_2(organism, next_cells);
                        break;
                    default:
                        ;
                    }
                    for (auto it : next_cells) {

                        //create a new organism
                        Organism *new_organism = new Organism(year_i, organism->getSpecies(), organism, it);
                        new_organism->setRandomDispersalAbility();
                        if (new_organism->isSuitable(current_environments, mask)){
                            new_organisms.push_back(new_organism);
                        }else{
                            delete new_organism;
                        }
                    }
                }
            }
            //LOG(DEBUG) << "new_organisms size is "<<new_organisms.size();
            for (auto it : new_organisms) {
                int id = it->getID();
                //species id, index
                organisms_in_current_year[s_it.first][id].push_back(it);
                all_organisms[year_i].push_back(it);
            }
            //LOG(DEBUG)<<"end to simulate organism by organism.";
        }

        LOG(DEBUG)<<"end to simulate organism by species. Count of species is " << actived_organisms.size();

        //Clean actived_organisms
        actived_organisms.clear();
        //unordered_map<Species*, unordered_map<int, vector<Organism*> *> *>
        for (auto sp_it =organisms_in_current_year.begin(); sp_it!=organisms_in_current_year.end();) {
            Species *species = sp_it->first;
            //for some wired extinction conditions.
            if ((sp_it->second.size() > 0) &&
                    ((species->getCurrentSpeciesExtinctionTimeSteps() < species->getSpeciesExtinctionTimeSteps())) &&
                    (sp_it->second.size() >= (species->getMaxSpeciesDistribution() * species->getSpeciesExtinctionThreaholdPercentage()))) {

                species->setMaxSpeciesDistribution((sp_it->second.size() > species->getMaxSpeciesDistribution()) ? sp_it->second.size() : species->getMaxSpeciesDistribution());
                if ((sp_it->second.size() <= species->getSpeciesExtinctionThreshold()) && ((int)year_i >= species->getSpeciationYears())) {
                    species->addCurrentSpeciesExtinctionTimeSteps();
                } else {
                    species->setCurrentSpeciesExtinctionTimeSteps(0);
                }
                sp_it++;
            } else {
                //if the species should extinct, remove all organisms and remove the species from organisms_in_current_year
                for (auto it : sp_it->second) {
                    it.second.clear();
                }
                sp_it->second.clear();
                sp_it->first->setDisappearedYearI(year_i);
                sp_it = organisms_in_current_year.erase(sp_it);
            }
        }
        LOG(DEBUG)<<"end to remove unsuitable organisms. organisms size is "<< organisms_in_current_year.size();

        //mark the group id for every organisms in this year, seperated by species id;
        LOG(DEBUG)<<"Begin to mark the group id, and detect the speciation.";
        for (auto sp_it : organisms_in_current_year) {
            //printf("Species ID:%u\n", sp_it.first->getID());
            unordered_map<int, vector<Organism*> > organisms = sp_it.second;
            Species *species = sp_it.first;
            int  current_group_id = 1;
            //if ((int)year_i >= (species->getBurnInYear() + species->getSpeciationYears())) {
            if (true){
                //LOG(DEBUG)<<"Begin to mark the organism.";
                int unmarked_id = getUnmarkedID(organisms);

                while (unmarked_id != -1) {
                    //LOG(DEBUG)<<"Unmarked organism is "<<unmarked_organism->getX() <<", "<<unmarked_organism->getY()
                    //      <<" dispersal ability is "<<unmarked_organism->getDispersalAbility()<<". current_group_id is "<<current_group_id;
                    int dispersal_ability = -1;
                    for (auto it : organisms[unmarked_id]){
                        dispersal_ability = (dispersal_ability>it->getDispersalAbility())?dispersal_ability:it->getDispersalAbility();
                    }
                    markJointOrganism(current_group_id, unmarked_id, dispersal_ability, organisms);
                    current_group_id++;
                    //LOG(DEBUG)<<"NEW GROUP ADDED";
                    unmarked_id = getUnmarkedID(organisms);

                }
                //LOG(DEBUG)<<"End to mark the organism. " << current_group_id-1<<" groups were found.";
                //detect the speciation
                int  temp_species_id = 1;
//              vector<string> group_output;
                LOG(DEBUG)<<"Begin to detect speciation.";
                for (int  group_id_1 = 1; group_id_1 < current_group_id - 1; group_id_1++) {
                    LOG(DEBUG)<<"getTempSpeciesID 1 for group "<<group_id_1 <<" current_group_id is "<<current_group_id;
                    int  temp_species_id_1 = getTempSpeciesID(group_id_1, organisms);
                    for (int  group_id_2 = group_id_1 + 1; group_id_2 < current_group_id; group_id_2++) {
                        LOG(DEBUG)<<"getTempSpeciesID 2 for group "<<group_id_2 <<" current_group_id is "<<current_group_id;
                        int  temp_species_id_2 = getTempSpeciesID(group_id_2, organisms);
                        //if both groups were marked, and they have the same id, skip it.
                        if ((temp_species_id_1 != 0) && (temp_species_id_2 != 0) && (temp_species_id_1 == temp_species_id_2)) {
                            continue;
                        }
                        LOG(DEBUG)<<"get min_divided_year.";
                        int min_divided_year = getMinDividedYear(sp_it.first->getSpeciationYears(), group_id_1, group_id_2, organisms, year_i);
                        LOG(DEBUG)<<"min_divided_year is "<<min_divided_year;

                        if (min_divided_year >= species->getSpeciationYears()) {
                            LOG(DEBUG)<<"mark species id with new id.";
                            //if a speciation happened, marked them with two ids if they were not marked.
                            if (temp_species_id_1 == 0) {
                                markedSpeciesID(group_id_1, temp_species_id, organisms);
                                temp_species_id_1 = temp_species_id;
                                temp_species_id++;
                            }
                            if (temp_species_id_2 == 0) {
                                markedSpeciesID(group_id_2, temp_species_id, organisms);
                                temp_species_id_2 = temp_species_id;
                                temp_species_id++;
                            }
                            LOG(DEBUG)<<"end mark species id with new id.";
                        } else {
                            LOG(DEBUG)<<"mark species id with old id.";
                            //if there is not speciation, marked them with the same id
                            int  t_id = (temp_species_id_1 == 0) ? temp_species_id_2 : temp_species_id_1;
                            t_id = (t_id == 0) ? temp_species_id : t_id;
                            markedSpeciesID(group_id_1, t_id, organisms);
                            temp_species_id_1 = t_id;
                            markedSpeciesID(group_id_2, t_id, organisms);
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
        LOG(DEBUG)<<"end to mark the group id, and detect the speciation. species size is "<<organisms_in_current_year.size();

        LOG(DEBUG)<<"Begin to rebuild the organism structure in this year";
        for (auto sp_it = organisms_in_current_year.begin(); sp_it!= organisms_in_current_year.end();) {
            LOG(DEBUG)<<"get all organisms in this year";
            unordered_map<int, vector<Organism*> > organisms = sp_it->second;
            //count all the species
            unordered_map<int, int> species_ids;
            set<int> temp_species_ids;
            for (auto c_it : organisms) {
                LOG(DEBUG)<<"go from the first species in this year";
                if (c_it.second.size() > 0) {
                    Organism *organism_item = c_it.second.front();
                    temp_species_ids.insert(organism_item->getTempSpeciesId());
                }
            }
            int  i = 1;
            for (auto it : temp_species_ids) {
                species_ids[it] = i++;
            }
            temp_species_ids.clear();
            //if there is a new species, remove the old one from the array and put the new species in.
            if (species_ids.size() > 1) {
                for (auto sp_id_it : species_ids) {
                    Species *new_species = new Species(sp_id_it.second, sp_it->first, year_i);
                    //createSpeciesFolder(new_species, false);
                    all_species[new_species->getIDWithParentID()] = new_species;
                    unordered_map<int, vector<Organism*> > new_map;

                    for (auto c_it : organisms) {
                        for (auto o_it : c_it.second) {
                            if (o_it->getTempSpeciesId() == sp_id_it.first) {
                                new_map[c_it.first].push_back(o_it);
                                o_it->setSpecies(new_species);
                            }
                        }
                    }
                    organisms_in_current_year[new_species] = new_map;

                }
                for (auto it_t : sp_it->second){
                    it_t.second.clear();
                }

                sp_it->second.clear();
                //sp_it->first->setDisappearedYearI(year_i);
                sp_it = organisms_in_current_year.erase(sp_it);

            } else {
                sp_it++;
            }
            species_ids.clear();
        }
        LOG(DEBUG)<<"x11";

        LOG(DEBUG)<<"End to rebuild the organism structure in this year. organisms size is "<<organisms_in_current_year.size();
        LOG(DEBUG)<<"begin to generate group maps";
        unordered_map<Species*, ISEA*> group_maps;
        for (auto sp_it : organisms_in_current_year) {
            Species *species = sp_it.first;
            if (group_maps.find(species) == group_maps.end()) {
                group_maps[sp_it.first] = new ISEA();
            }
            if (sp_it.second.size() > 0) {
                for (auto o_id : sp_it.second) {
                    if (o_id.second.size() > 0) {
                        group_maps[sp_it.first]->setValue(o_id.second.front()->getID(), o_id.second.front()->getGroupId());
                    }
                }
            } else {
                delete group_maps[sp_it.first];
            }

        }

        saveGroupmap(year_i, group_maps);
        for (auto it : group_maps){
            if (it.second){
                delete it.second;
            }
        }
        group_maps.clear();

        //remove the useless organism
        LOG(DEBUG) << "Remove the useless organisms. Before removing, Memory usage:" << CommonFun::getCurrentRSS(1);
        Species *sp_it = all_species.begin()->second;
        if ((int) year_i < sp_it->getDispersalSpeed()) {
            continue;
        }
        int speciation_year = sp_it->getSpeciationYears();
        bool is_remove_previous_span = false;
        bool is_remove_previous_speciation = false;
        if ((int) year_i < (sp_it->getBurnInYear() - sp_it->getSpeciationYears())) {
            is_remove_previous_span = true;
        }
        if ((int) year_i > sp_it->getBurnInYear()) {
            is_remove_previous_speciation = true;
        }
        int removed_year_i = -1;
        if (is_remove_previous_span) {
            removed_year_i = (int) year_i - sp_it->getDispersalSpeed();
        }
        if (is_remove_previous_speciation) {
            removed_year_i = (int) year_i - sp_it->getDispersalSpeed() - speciation_year;
        }
        if (removed_year_i >= 0) {
            //LOG(DEBUG) << "Remove organism at " << removed_year_i;
            //for (auto it : *all_organisms) {
                //LOG(DEBUG) << "all_organisms keys " << it.first;
            //}
            for (auto it : all_organisms[removed_year_i]) {
                delete it;
            }
            all_organisms[removed_year_i].clear();
            all_organisms.erase(removed_year_i);

            //              LOG(DEBUG)<<"Set the parent to NULL for year "<<removed_year + sp_it->getDispersalSpeed();
            for (auto it1 : all_organisms[removed_year_i + sp_it->getDispersalSpeed()]) {
                it1->setParent(NULL);
            }
            //              LOG(DEBUG)<<"Done!";
        }

        current_environments.clear();
        LOG(DEBUG)<<"Remove the useless organisms. After  removing, Memory usage:"<<CommonFun::getCurrentRSS(1);

        malloc_trim(0);
        if (all_organisms.find(year_i)==all_organisms.end()){
            break;
        }
    }
    //clean the last organisms_in_current_year
    for (auto it : organisms_in_current_year){
        for (auto it2 : it.second){
            it2.second.clear();
        }
        it.second.clear();
    }
    organisms_in_current_year.clear();

    generateSpeciationInfo();
    malloc_trim(0);
    //CommonFun::executeSQL("CREATE INDEX idx_year ON map (year)", log_db);
    return 0;
}
void Simulation::generateSpeciationInfo() {
    vector<Species*> roots;
    for (auto it : all_species) {
        Species *sp_it = it.second;
        //if the species is the root species, make the node from it.
        if (sp_it->getAppearedYearI() == 0) {
            sp_it->markNode();
            roots.push_back(sp_it);
        }
    }

    for (auto sp_it : roots) {
        string tree = sp_it->getNewickTree(true, false);
        string sql = "INSERT INTO trees (TYPE, CONTENT) VALUES ('NEWICK', " + CommonFun::quoteSql(&tree) + ");";
        CommonFun::executeSQL(sql, log_db, true);
        vector<string> htmltree = sp_it->getHTMLTree();
        tree = boost::algorithm::join(htmltree, " ");
        sql = "INSERT INTO trees (TYPE, CONTENT) VALUES ('HTML', " + CommonFun::quoteSql(&tree) + ");";
        CommonFun::executeSQL(sql, log_db, true);
        htmltree.clear();
    }
    roots.clear();
}
int Simulation::getTempSpeciesID(int group_id, unordered_map<int, vector<Organism*> > &organisms) {
    for (auto c_it : organisms) {
        for (auto o_it : c_it.second) {
            if (o_it->getGroupId() == group_id) {
                return o_it->getTempSpeciesId();
            }
        }
    }
    return 0;
}
void Simulation::markedSpeciesID(int group_id, int temp_species_id, unordered_map<int, vector<Organism*> > &organisms) {
    //LOG(DEBUG)<<"Size of organism 1 is "<<organisms->size();
    for (auto c_it : organisms) {
        //LOG(DEBUG)<<"Size of organism 2 is "<<c_it.second.size();
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
int Simulation::getMinDividedYear(int speciation_year, int  group_id_1, int group_id_2,
        unordered_map<int, vector<Organism*> > &organisms, int current_year_i){
    /*
    int total = 0;
    for (auto c_it : *organisms){
        total += c_it.second.size();
    }
    LOG(INFO) << "TOTAL ORGANISMS ARE "<<total;
    int v;
    clock_t start, end;
    double time_taken;
    start = clock();
    vector<Organism*> group_1;
    vector<Organism*> group_2;
    for (auto c_it : organisms) {
        for (auto o_it : c_it.second) {
            if (o_it->getGroupId() == group_id_1) {
                group_1.push_back(o_it);
            } else if (o_it->getGroupId() == group_id_2) {
                group_2.push_back(o_it);
            }
        }
    }
    LOG(INFO) << "GROUP 1 "<<group_1->size() <<", GROUP 2 "<< group_2->size();
    v = getMinDividedYear_old(speciation_year, group_1, group_2, organisms, current_year_i);
    end = clock();
    time_taken = double(end - start) / double(CLOCKS_PER_SEC);
    t1 += time_taken;
    LOG(INFO) << "OLD METHODS TOOK " << time_taken << " SEC, AND VALUE IS " << v << ", TOTAL TIME IS " << t1;

    start = clock();
    vector<Organism*> group_1;
    vector<Organism*> group_2;
    for (auto c_it : organisms) {
        for (auto o_it : c_it.second) {
            if (o_it->getGroupId() == group_id_1) {
                group_1.push_back(o_it);
            } else if (o_it->getGroupId() == group_id_2) {
                group_2.push_back(o_it);
            }
        }
    }
    LOG(INFO) << "GROUP 1 "<<group_1->size() <<", GROUP 2 "<< group_2->size();
    v = getMinDividedYear_Disjoint(speciation_year, group_1, group_2, organisms, current_year_i);
    end = clock();
    time_taken = double(end - start) / double(CLOCKS_PER_SEC);
    t2 += time_taken;
    LOG(INFO) << "NEW METHODS TOOK " << time_taken << " SEC, AND VALUE IS " << v << ", TOTAL TIME IS " << t2;
    return v;
    */

    vector<Organism*> group_1;
    vector<Organism*> group_2;
    for (auto c_it : organisms) {
        for (auto o_it : c_it.second) {
            if (o_it->getGroupId() == group_id_1) {
                group_1.push_back(o_it);
            } else if (o_it->getGroupId() == group_id_2) {
                group_2.push_back(o_it);
            }
        }
    }
    return getMinDividedYear_Disjoint(speciation_year, group_1, group_2, current_year_i);
}
int Simulation::getMinDividedYear_Disjoint(int speciation_year, vector<Organism*> &group_1,
        vector<Organism*> &group_2, int current_year_i) {
    int nearest_divided_year_i = 1;
    //LOG(DEBUG)<<"2";
    while (nearest_divided_year_i<speciation_year) {
        bool is_end = false;
        set<int> parent_1;
        set<int> parent_2;
        for (auto it : group_1) {
            parent_1.insert(it->getParent()->getGroupId());
        }
        for (auto it : group_2) {
            parent_2.insert(it->getParent()->getGroupId());
        }
        set<int> intersect;
        set_intersection(parent_1.begin(), parent_1.end(), parent_2.begin(), parent_2.end(), inserter(intersect, intersect.begin()));
        if (intersect.size()>0){
            return nearest_divided_year_i;
        }
        nearest_divided_year_i++;

        vector<Organism*> group_1_t;
        vector<Organism*> group_2_t;
        for (auto it : group_1) {
            if (it->getParent() == NULL) {
                is_end = true;
                break;
            }
            group_1_t.push_back(it->getParent());
        }
        if (!is_end) {
            for (auto it : group_2) {
                if (it->getParent() == NULL) {
                    is_end = true;
                    break;
                }
                group_2_t.push_back(it->getParent());
            }
        }
        group_1 = group_1_t;
        group_2 = group_2_t;

        if (is_end){
            return  nearest_divided_year_i;
        }
    }
    return nearest_divided_year_i;
}
int Simulation::getMinDividedYear_old(int speciation_year, vector<Organism*> group_1, vector<Organism*> group_2, int current_year_i) {
    int nearest_divided_year_i = 0;
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
void Simulation::getNeighbors(int id, int distance, set<int> &cells) {
    set<int> handled_ids;
    neighborInfo->getNeighborByID(id, distance, cells, handled_ids);
}
void Simulation::markJointOrganism(int p_group_id, int unmarked_id, int dispersal_ability, unordered_map<int, vector<Organism*> > &organisms) {

    if (dispersal_ability == 0) {
        dispersal_ability = 1;
    }
    //LOG(DEBUG)<<"id="<<id<<" p_dispersal_ability="<<p_dispersal_ability;
    set<int> neighbors;
    getNeighbors(unmarked_id, dispersal_ability, neighbors);
    //string n_ids = "";
    //string n_ids_x = "";
    //string n_ids_y = "";
    for (int n_id : neighbors) {
        //n_ids += "," + to_string(n_id);
        if (organisms.find(n_id) == organisms.end()) {
            continue;
        }
        //n_ids_x += "," + to_string(n_id);
        vector<Organism*> organisms_in_id = organisms[n_id];
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
}

int Simulation::getUnmarkedID(unordered_map<int, vector<Organism*>> &organisms) {
    for (auto p_it : organisms) {
        if (p_it.second.front()->getGroupId()==0){
            return p_it.first;
        }
    }
    return -1;
}
unordered_map<string, ISEA*> Simulation::getEnvironmentMap(int p_year) {
    unordered_map<string, ISEA*> maps;
    for (auto item : environments) {
        ISEA *layer = item.second->getValues(p_year);
        maps[item.first] = layer;
    }
    return maps;
}


void Simulation::getDispersalMap_2(Organism *organism, set<int> &new_cells) {
    int p_dispersal_ability = organism->getDispersalAbility();
    if (organism->getNumOfPath() == -1) {
        int id = organism->getID();
        //LOG(DEBUG) << "Looking for neighbors for " << id << ", which dispersal ability is " << p_dispersal_ability << ".";
        getNeighbors(id, p_dispersal_ability, new_cells);
        //LOG(DEBUG)<<new_cells.size() <<" neighbors were found.";
    }
}
Simulation::~Simulation() {

    LOG(INFO)<<"Cleaning everything in the simulation";
    sqlite3_close(log_db);
    //unordered_map<int, vector<Organism*> *>

    for (auto it1 : all_organisms){
        for (auto it2 : it1.second){
            if (it2){
                LOG(DEBUG)<<"z3.1 "<<it2;
                delete it2;
            }
        }
    }
    for (auto it : all_species){
        if (it.second){
            delete it.second;
        }
    }
}


