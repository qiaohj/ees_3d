library(rgdal)
library(raster)
library(stringr)
library(dplyr)
library(RSQLite)
library(DBI)


logdb<-"/home/huijieqiao/git/ees_3d_data/TEST/Results/259_GOOD_BROAD/259_GOOD_BROAD.sqlite"
mydb <- dbConnect(RSQLite::SQLite(), logdb)

suitable<-dbReadTable(mydb, "suitable")

map<-dbReadTable(mydb, "map")
dbDisconnect(mydb)
shape <- readOGR(dsn = "/home/huijieqiao/git/ees_3d_data/ISEA3H8/isea3hGen/outputfiles", layer = "isea3h8p")
shape_t<-shape
shape_t@coords<-subset(shape_t@coords, shape_t$global_id %in% suitable$ID)
shape_t@data<-subset(shape_t@data, shape_t$global_id %in% suitable$ID)
shape_t@data<-merge(shape_t@data, suitable, by.x="global_id", by.y="ID")
writeOGR(shape_t, dsn = "/home/huijieqiao/git/ees_3d_data/ISEA3H8/test", 
         layer = "test2", driver="ESRI Shapefile", overwrite_layer=T)
table(map$YEAR)
y<-1101
for (y in c(1199:0)){
  
  item <- map[which(map$YEAR==y),]
  shape_t<-shape
  shape_t@coords<-subset(shape_t@coords, shape_t$global_id %in% item$ID)
  shape_t@data<-subset(shape_t@data, shape_t$global_id %in% item$ID)
  shape_t@data<-merge(shape_t@data, item, by.x="global_id", by.y="ID")
  plot(shape_t, col=item$group_id, cex=0.2)
  table(item$group_id)
  x<-readline(prompt=sprintf("Year is %d, Found %d groups. (X=exit): ", y, length(unique(item$group_id))))
  if (tolower(x)=="x"){
    break()
  }
}

if (F){
        
        
        suitable<-read.csv("/home/huijieqiao/git/ees_3d_data/TEST/Results/31832_GOOD_BROAD/suitable.csv", head=T, sep=",")
        
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
}