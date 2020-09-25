library(dplyr)
library(MASS)
library(ggplot2)
library(RSQLite)
library(DBI)
setwd("~/git/ees_3d/R/Niche_Center_G_E")
target_dir<-"/media/huijieqiao/Butterfly/Niche_Center_E_G/Results/"
base_db<-sprintf("%s/../Configurations/conf.sqlite", target_dir)
envdb <- dbConnect(RSQLite::SQLite(), base_db)
simulations<-dbReadTable(envdb, "simulations")
dbDisconnect(envdb)

base_db<-sprintf("%s/../Configurations/env_Hadley3D.sqlite", target_dir)
envdb <- dbConnect(RSQLite::SQLite(), base_db)
v_min_temp<-dbReadTable(envdb, "Debiased_Minimum_Monthly_Temperature")
v_max_temp<-dbReadTable(envdb, "Debiased_Maximum_Monthly_Temperature")
v_max_prec<-dbReadTable(envdb, "Debiased_Maximum_Monthly_Precipitation")
dbDisconnect(envdb)

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
ll<-readRDS("/home/huijieqiao/git/ees_3d_data/niche_conservatism/Data/ENV/mask_df.rda")
for (i in c(1:length(items))){
  item<-items[i]
  item_s<-strsplit(item, "_")[[1]]
  ID<-as.numeric(item_s[1])
  DA<-item_s[2]
  NB<-item_s[3]
  
  print(paste(i, length(items), item))
  log_file<-sprintf("%s/%s/%s.log", target_dir, item, item)
  if (!file.exists(log_file)){
    next()
  }
  s<-simulations%>%dplyr::filter((global_id==ID)&(da==DA)&(nb==NB))
  nb_v<-strsplit(s[1, "nb_v"], "\\|")[[1]]
  seed<-ll%>%dplyr::filter((global_id==ID)&(Y==0))
  temp_str<-as.numeric(strsplit(nb_v[1], ",")[[1]])
  temp<-sum(temp_str)/2
  prec_str<-as.numeric(strsplit(nb_v[3], ",")[[1]])
  prec<-sum(prec_str)/2
  fn_nb_center<-c(temp, prec)
  fn_geo_center<-c(seed$lon, seed$lat)
  df<-read.csv(log_file, head=F, stringsAsFactors = F)
  colnames(df)<-c("Y", "GLOBAL_ID", "GROUP_ID", "N", "SP_ID", "IS_SUITABLE")
  df<-df%>%dplyr::filter(IS_SUITABLE==1)
  if (nrow(df)==0){
    next()
  }
  df_with_env<-inner_join(df, ll, by=c("GLOBAL_ID"="global_id", "Y"="Y"))
  
  sp_ids<-data.frame(SP_ID=as.character(unique(df_with_env$SP_ID)),
                     ST_YEAR=NA, END_YEAR=NA)
  sp_ids<-df_with_env%>%dplyr::group_by(SP_ID)%>%
    dplyr::summarise(ST_YEAR=max(Y),
                   END_YEAR=min(Y))
  for (year in c(1198:0)){
    df_item<-df_with_env%>%dplyr::filter(Y==year)
    it (nrow(df_item)==0){
      next()
    }
    sp<-df_item[1, "SP_ID"]
    for (sp in unique(df_item$SP_ID)){
      sp_item<-df_item%>%dplyr::filter(SP_ID==sp)
      sp_item$TEMP<-(sp_item$MIN_TEMP+sp_item$MAX_TEMP)/2
      geo_center_gravity<-c(mean(sp_item$lon), mean(sp_item$lat))
      fit <- cov.rob(sp_item[, c("lon", "lat")], 
                     quantile.used=NDquntil(nrow(sp_item), 0.95),  
                     method = "mve")
      geo_center_mve<-fit$center
      
      niche_center_gravity<-c(mean(sp_item$TEMP), mean(sp_item$MAX_PREC))
      fit <- cov.rob(sp_item[, c("TEMP", "MAX_PREC")], 
                     quantile.used=NDquntil(nrow(sp_item), 0.95),  
                     method = "mve")
      niche_center_mve<-fit$center
      sp_item<-sp_item%>%dplyr::rowwise()%>%
        dplyr::mutate(
          dist_fn_geo_center=e_dist(lon, lat, fn_geo_center[1], fn_geo_center[2])
        )
      
      sp_item$dist_fn_geo_center<-dist()
      if (F){
        plot(sp_item$lon, sp_item$lat, col=sp_item$N)
        plot(sp_item$dist_fn_geo_center, sp_item$N)
        ggplot(sp_item)+
          geom_point(aes(x=lon, y=lat, color=N))+
          geom_point(data=seed, aes(x=lon, y=lat), color="red")
      }
      
    }
  }
}
