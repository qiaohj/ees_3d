library(rgdal)
library(raster)
library(stringr)
library(dplyr)
library(RSQLite)
library(DBI)

da_list<-data.frame(da=c("GOOD", "POOR"), 
                    v=c("0.376596738,0.724239343,0.976679685,0.9995805560000001,1.0",
                        "0.710669364,0.999605965,0.9999990249999999,0.999999999299,1.0"),
                    stringsAsFactors = F)
nb_range<-list("BROAD"=c(20, 2),
               "MODERATE"=c(10,1),
               "NARROW"=c(5,0.5))

base<-"/home/huijieqiao/git/ees_3d_data/ISEA3H8"
base_db<-sprintf("%s/SQLITE/env_Hadley3D.sqlite", base)
envdb <- dbConnect(RSQLite::SQLite(), base_db)
mask<-dbReadTable(envdb, "mask")

v_mean_temp<-dbReadTable(envdb, "Debiased_Mean_Annual_Temperature")
v_mean_temp<-v_mean_temp[which(v_mean_temp$year==0),]
v_mean_prec<-dbReadTable(envdb, "Debiased_Mean_Annual_Precipitation")
v_mean_prec<-v_mean_prec[which(v_mean_prec$year==0),]

dbDisconnect(envdb)
mask$random_index<-sample(nrow(mask))

simulations<-NULL
i=1
id = 1
for (i in c(1:nrow(mask))){
  print(paste(i, nrow(mask)))
  item_t<-mask[i,]
  da<-da_list$da[1]
  for (da in da_list$da){
    nb<-names(nb_range)[1]
    for (nb in names(nb_range)){
      
      item<-item_t[, c("global_id", "random_index")]
      item$id<-id
      item<-item[, c("id", "global_id", "random_index")]
      item$label<-paste(item$global_id, da, nb, sep="_")
      id<-id+1
      item$da<-da
      item$nb<-nb
      nb1_min<-v_mean_temp[
        which(v_mean_temp$global_id==item$global_id), "v"]-nb_range[[nb]][1]/2
      nb1_max<-v_mean_temp[
        which(v_mean_temp$global_id==item$global_id), "v"]+nb_range[[nb]][1]/2
      
      nb2_min<-v_mean_prec[
        which(v_mean_prec$global_id==item$global_id), "v"]-nb_range[[nb]][2]/2
      nb2_max<-v_mean_prec[
        which(v_mean_prec$global_id==item$global_id), "v"]+nb_range[[nb]][2]/2
      item$nb_v<-paste(paste(nb1_min, nb1_max, sep=","), 
                       paste(nb2_min, nb2_max, sep=","),
                       sep="|")
      item$dispersal_ability<-da_list[which(da_list$da==da), "v"]
      item$dispersal_speed<-1
      item$dispersal_method<-2
      item$number_of_path<--1
      item$speciation_years<-100
      item$species_extinction_threshold<-0
      item$species_extinction_time_steps<-1
      item$species_extinction_threahold_percentage<-0.8
      item$group_extinction_threshold<-0
      item$initial_seeds<-id
      item$environments<-("Debiased_Mean_Annual_Temperature,Debiased_Mean_Annual_Precipitation")
      item$from<-1200
      item$to<-0
      item$step<--1
      item$mask<-"mask"
      item$burn_in_year<-0
      if (is.null(simulations)){
        simulations<-item
      }else{
        simulations<-bind_rows(simulations, item)
      }
    }
  }
}
dim(simulations)
head(simulations)
simulations$is_run<-0
simulations[which(simulations$random_index<=1000), "is_run"]<-1
simulations[which(simulations$is_run==1),]
mydb <- dbConnect(RSQLite::SQLite(), "/home/huijieqiao/git/ees_3d_data/TEST/conf.sqlite")
dbWriteTable(mydb, "simulations", simulations, overwrite=T)
dbDisconnect(mydb)
if (F){
  field.types=list(global_id="INTEGER",
                   random_index="INTEGER",
                   id="INTEGER",
                   label="TEXT",
                   da="TEXT",
                   nb="TEXT", 
                   nb_v="TEXT", 
                   dispersal_ability="TEXT",       
                   dispersal_speed="INTEGER",
                   dispersal_method="INTEGER",
                   number_of_path="INTEGER",
                   speciation_years="INTEGER",
                   species_extinction_threshold="INTEGER", 
                   species_extinction_time_steps="INTEGER",
                   species_extinction_threahold_percentage="REAL",
                   group_extinction_threshold="INTEGER",
                   initial_seeds="INTEGER",
                   environments="TEXT",
                   total_years="INTEGER",
                   mask="TEXT",
                   burn_in_year="INTEGER",
                   is_run="INTEGER"
  )
}


