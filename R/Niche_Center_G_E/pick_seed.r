library(dplyr)
library(rgdal)
library(raster)
setwd("~/git/ees_3d/R/Niche_Center_G_E")
df<-readRDS("../../../ees_3d_data/niche_conservatism/Data/stat.rda")
mask<-readRDS("../../../ees_3d_data/niche_conservatism/Data/ENV/mask_df.rda")
df_sub<-df%>%dplyr::filter((EVO_TYPE==1)&(between(N_SPECIATION, 50, 150)))
length(unique(df_sub$SEED_ID))
dim(df_sub)
seeds<-unique(df_sub$SEED_ID)
seeds_df<-mask%>%dplyr::filter((global_id %in% seeds)&(Y==0))
shape <- readOGR(dsn = "/home/huijieqiao/git/ees_3d_data/SMART_SPECIES/ISEA3H8/isea3hGen/outputfiles", layer = "isea3h8p")
shape_t<-shape
shape_t@coords<-subset(shape_t@coords, shape_t$global_id %in% seeds)
shape_t@data<-subset(shape_t@data, shape_t$global_id %in% seeds)
shape_t@data$global_id<-as.numeric(as.character(shape_t@data$global_id))
writeOGR(shape_t, dsn = "/home/huijieqiao/git/ees_3d_data/Niche_Center_G_E/Shape", 
         layer = "seeds", driver="ESRI Shapefile", overwrite_layer=T)


seeds_df_SA<-seeds_df%>%dplyr::filter(between(lon, -80, -30)&between(lat, -40, 10))
shape_t<-shape
shape_t@coords<-subset(shape_t@coords, shape_t$global_id %in% seeds_df_SA$global_id)
shape_t@data<-subset(shape_t@data, shape_t$global_id %in% seeds_df_SA$global_id)
shape_t@data$global_id<-as.numeric(as.character(shape_t@data$global_id))
writeOGR(shape_t, dsn = "/home/huijieqiao/git/ees_3d_data/Niche_Center_G_E/Shape", 
         layer = "seeds_SA", driver="ESRI Shapefile", overwrite_layer=T)


#random_seed<-seeds[sample(length(seeds), 10)]
print(paste(seeds_df_SA$global_id, sep=",", collapse = ","))
#update simulations set is_run=0
#update simulations set is_run=1 where 
#global_id in (9244,9648,9735,10049,10219,10311,
#10391,10393,10636,10641,10715,11035,11124,11204,
#11454,11462,11539,11591,11674,11936,12186,12198,
#12332,12335,12337,12338,12499,12611,12665,12745,
#12775,12977,13020,13099,39397,39805,39807,39889,39897,40051,40112)


#update simulations set `from`=1200
#update simulations set label=global_id||"_"||da||"_"||nb||"_"||evo_type||"_"||niche_envolution_individual_ratio
#DELETE from simulations where evo_type<>1

library(RSQLite)
library(DBI)
setwd("~/git/ees_3d/R/Niche_Center_G_E")
target_dir<-"/media/huijieqiao/Butterfly/Niche_Center_E_G/Results/"
base_db<-sprintf("%s/../Configurations/conf_bak.sqlite", target_dir)
envdb <- dbConnect(RSQLite::SQLite(), base_db)
simulations<-dbReadTable(envdb, "simulations")
dbDisconnect(envdb)

base_db<-sprintf("%s/../Configurations/env_Hadley3D.sqlite", target_dir)
envdb <- dbConnect(RSQLite::SQLite(), base_db)
v_min_temp<-dbReadTable(envdb, "Debiased_Minimum_Monthly_Temperature")
v_max_temp<-dbReadTable(envdb, "Debiased_Maximum_Monthly_Temperature")
v_max_prec<-dbReadTable(envdb, "Debiased_Maximum_Monthly_Precipitation")
dbDisconnect(envdb)
nb_range<-list("BROAD"=c(60, 10),
               "NARROW"=c(40, 5))

simulations<-simulations[which(simulations$evo_type==1),]

simulations$id<-1
simulations$random_index<-1
simulations$nb_v<-NA
simulations<-unique(simulations)
v_min_temp<-v_min_temp%>%dplyr::filter(year==1200)
v_max_temp<-v_max_temp%>%dplyr::filter(year==1200)
v_max_prec<-v_max_prec%>%dplyr::filter(year==1200)
simulations$label<-paste(simulations$global_id, simulations$da, simulations$nb, sep="_")
for (i in c(1:nrow(simulations))){
  print(i)
  s<-simulations[i,]
  min_temp<-v_min_temp%>%dplyr::filter(global_id==s$global_id)
  max_temp<-v_max_temp%>%dplyr::filter(global_id==s$global_id)
  nb_temp<-mean(c(min_temp$v, max_temp$v))
  nb1_min<-nb_temp-nb_range[[s$nb]][1]/2
  nb1_max<-nb_temp+nb_range[[s$nb]][1]/2
  nb_prec<-(v_max_prec %>% dplyr::filter((global_id==s$global_id)))$v
  nb2_min<-nb_prec-nb_range[[s$nb]][2]/2
  nb2_max<-nb_prec+nb_range[[s$nb]][2]/2
  
  simulations[i,]$nb_v<-paste(paste(nb1_min, nb1_max, sep=","), 
                   paste(nb1_min, nb1_max, sep=","),
                   paste(nb2_min, nb2_max, sep=","),
                   sep="|")
}

base_db<-sprintf("%s/../Configurations/conf.sqlite", target_dir)
mydb <- dbConnect(RSQLite::SQLite(), base_db)
dbWriteTable(mydb, "simulations", simulations, overwrite=T)
dbDisconnect(mydb)
