library(ggplot2)
library(dplyr)
library(raster)
library(dplyr)
library(MASS)
library(ggplot2)
library(RSQLite)
library(DBI)
library(data.table)
library(dprep)
setwd("~/git/ees_3d/R/Niche_Center_G_E")
target_dir<-"/media/huijieqiao/Butterfly/Niche_Center_E_G/Results/"
print("Loading configuration ...")
base_db<-sprintf("%s/../Configurations/conf.sqlite", target_dir)
envdb <- dbConnect(RSQLite::SQLite(), base_db)
simulations<-dbReadTable(envdb, "simulations")
dbDisconnect(envdb)

source("addEllipse.R")
source("genCircle.R")

items<-list.dirs(target_dir, full.names=F)
item<-items[3]

print("Loading lon/lat information ...")
ll<-readRDS("/home/huijieqiao/git/ees_3d_data/niche_conservatism/Data/ENV/mask_df.rda")
ll$TEMP<-(ll$MIN_TEMP+ll$MAX_TEMP)/2

ll_start<-ll%>%dplyr::filter(Y==1200)
i=1
for (i in c(1:length(items))){
  #for (i in c(12:12)){
  item<-items[i]
  item_s<-strsplit(item, "_")[[1]]
  ID<-as.numeric(item_s[1])
  DA<-item_s[2]
  NB<-item_s[3]
  print(paste(i, length(items), item))
  target_rda<-sprintf("%s/%s/center_density.rda", target_dir, item)
  if (!file.exists(target_rda)){
    print(paste("skip", target_rda))
    next()
  }
  df<-readRDS(target_rda)
  df<-df%>%dplyr::rowwise()%>%
    dplyr::mutate(
      dist_fn_geo_center=gcd.slc(lon, lat, 
                                fn_geo_center_x, 
                                fn_geo_center_y),
      dist_fn_nb_center=m_dist(TEMP, MAX_PREC, 
                               fn_nb_center_x, 
                               fn_nb_center_y, S),
      dist_geo_center_gravity=gcd.slc(lon, lat, 
                                     geo_center_gravity_x, 
                                     geo_center_gravity_y),
      dist_geo_center_mve=gcd.slc(lon, lat, 
                                 geo_center_mve_x, 
                                 geo_center_mve_y),
      dist_niche_center_gravity=m_dist(TEMP, MAX_PREC, 
                                       niche_center_gravity_x, 
                                       niche_center_gravity_y, S),
      dist_niche_center_mve=m_dist(TEMP, MAX_PREC, 
                                   niche_center_mve_x, 
                                   niche_center_mve_y, S)
    )
}
