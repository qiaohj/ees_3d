library(rgdal)
library(raster)
library(stringr)
library(dplyr)
suitable<-read.csv("/home/huijieqiao/git/ees_3d_data/TEST/Results/10382_GOOD_BROAD/suitable.csv", head=T, sep=",")
shape <- readOGR(dsn = "/home/huijieqiao/git/ees_3d_data/ISEA3H8/isea3hGen/outputfiles", layer = "isea3h8")
shape_t<-shape
shape_t@polygons<-subset(shape_t@polygons, shape_t$global_id %in% suitable$global_id)
shape_t@data<-subset(shape_t@data, shape_t$global_id %in% suitable$global_id)
writeOGR(shape_t, dsn = "/home/huijieqiao/git/ees_3d_data/ISEA3H8/test", 
         layer = "test", driver="ESRI Shapefile", overwrite_layer=T)
