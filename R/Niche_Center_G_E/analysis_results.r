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

if (F){
  base_db<-sprintf("%s/../Configurations/env_Hadley3D.sqlite", target_dir)
  envdb <- dbConnect(RSQLite::SQLite(), base_db)
  v_min_temp<-dbReadTable(envdb, "Debiased_Minimum_Monthly_Temperature")
  v_max_temp<-dbReadTable(envdb, "Debiased_Maximum_Monthly_Temperature")
  v_max_prec<-dbReadTable(envdb, "Debiased_Maximum_Monthly_Precipitation")
  dbDisconnect(envdb)
}
source("addEllipse.R")
source("genCircle.R")

items<-list.dirs(target_dir, full.names=F)
item<-items[3]
NDquntil <- function(nD, level) {
  n <- floor(nD * level)
  if (n > nD) 
    n <- nD
  return(n)
}
in_Ellipsoid <- stats::qchisq(0.95, 2)
e_dist<-function(x1, y1, x2, y2){
  sqrt((x1-x2)^2+(y1-y2)^2)
}
m_dist<-function(x1, y1, x2, y2, S){
  stats::mahalanobis(c(x1, y1), c(x2, y2), S)
}
gcd.slc <- function(long1, lat1, long2, lat2) {
  R <- 6371 # Earth mean radius [km]
  d <- acos(sin(lat1)*sin(lat2) + cos(lat1)*cos(lat2) * cos(long2-long1)) * R
  return(d) # Distance in km
}

print("Loading lon/lat information ...")
ll<-readRDS("/home/huijieqiao/git/ees_3d_data/niche_conservatism/Data/ENV/mask_df.rda")
ll$TEMP<-(ll$MIN_TEMP+ll$MAX_TEMP)/2
S <-  var(ll[, c("TEMP", "MAX_PREC")], na.rm = T)

znorm(ll$TEMP)
ll_start<-ll%>%dplyr::filter(Y==1200)
i=12
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
