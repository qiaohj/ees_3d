library(rgdal)
library(raster)
library(stringr)
library(dplyr)
library(RSQLite)
library(DBI)

suitable<-read.csv("/home/huijieqiao/git/ees_3d_data/TEST/Results/31832_GOOD_BROAD/suitable.csv", head=T, sep=",")
shape <- readOGR(dsn = "/home/huijieqiao/git/ees_3d_data/ISEA3H8/isea3hGen/outputfiles", layer = "isea3h8")
shape_t<-shape
shape_t@polygons<-subset(shape_t@polygons, shape_t$global_id %in% suitable$global_id)
shape_t@data<-subset(shape_t@data, shape_t$global_id %in% suitable$global_id)
writeOGR(shape_t, dsn = "/home/huijieqiao/git/ees_3d_data/ISEA3H8/test", 
         layer = "test", driver="ESRI Shapefile", overwrite_layer=T)

ids<-c(10218
       ,10219
       ,10220
       ,10299
       ,10300
       ,10301
       ,10302
       ,10380
       ,10381
       ,10382
       ,10383
       ,10384
       ,10462
       ,10463
       ,10464
       ,10465
       ,10544
       ,10545
       ,10546)
shape_t<-shape
shape_t@polygons<-subset(shape_t@polygons, shape_t$global_id %in% ids)
shape_t@data<-subset(shape_t@data, shape_t$global_id %in% ids)
writeOGR(shape_t, dsn = "/home/huijieqiao/git/ees_3d_data/ISEA3H8/test", 
         layer = "test", driver="ESRI Shapefile", overwrite_layer=T)


mydb <- dbConnect(RSQLite::SQLite(), "/home/huijieqiao/git/ees_3d_data/TEST/Results/31832_GOOD_BROAD/log.db")
map<-dbReadTable(mydb, "map")
dbDisconnect(mydb)
ids<-map[which(map$YEAR==400), "ID"]
shape_t<-shape
shape_t@polygons<-subset(shape_t@polygons, shape_t$global_id %in% ids)
shape_t@data<-subset(shape_t@data, shape_t$global_id %in% ids)
writeOGR(shape_t, dsn = "/home/huijieqiao/git/ees_3d_data/ISEA3H8/test", 
         layer = "test2", driver="ESRI Shapefile", overwrite_layer=T)
