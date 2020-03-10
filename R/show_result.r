library(rgdal)
library(raster)
library(stringr)
library(dplyr)
library(RSQLite)
library(DBI)


logdb<-"/home/huijieqiao/git/ees_3d_data/TEST/Results/259_GOOD_MODERATE/259_GOOD_MODERATE.sqlite"
mydb <- dbConnect(RSQLite::SQLite(), logdb)
trees<-dbReadTable(mydb, "trees")
suitable<-dbReadTable(mydb, "suitable")

map<-read.table(gsub("sqlite", "log", logdb), head=F, sep=",")
colnames(map)<-c("YEAR", "ID", "group_id", "sp_id")
dbDisconnect(mydb)
shape <- readOGR(dsn = "/home/huijieqiao/git/ees_3d_data/ISEA3H8/isea3hGen/outputfiles", layer = "isea3h8p")
shape_t<-shape
shape_t@coords<-subset(shape_t@coords, shape_t$global_id %in% suitable$ID)
shape_t@data<-subset(shape_t@data, shape_t$global_id %in% suitable$ID)
shape_t@data$global_id<-as.numeric(as.character(shape_t@data$global_id))
shape_t@data<-inner_join(shape_t@data, suitable, by=c("global_id"="ID"))
writeOGR(shape_t, dsn = "/home/huijieqiao/git/ees_3d_data/ISEA3H8/test", 
         layer = "1170", driver="ESRI Shapefile", overwrite_layer=T)
table(map$YEAR)
y<-1170
for (y in c(1199:0)){
  
  item <- map[which(map$YEAR==y),]
  shape_t<-shape
  shape_t@coords<-subset(shape_t@coords, shape_t$global_id %in% item$ID)
  shape_t@data$global_id<-as.numeric(as.character(shape_t@data$global_id))
  shape_t@data<-subset(shape_t@data, shape_t$global_id %in% item$ID)
  shape_t@data<-inner_join(shape_t@data, item, by=c("global_id"="ID"))
  plot(shape_t, col=rainbow(10)[item$group_id+1], cex=0.2)
  print(table(item$group_id))
  x<-readline(prompt=sprintf("Year is %d, Found %d groups. (X=exit): ", y, length(unique(item$group_id))))
  if (tolower(x)=="x"){
    break()
  }
}

library("ape")
library("phangorn")
library("phytools")
library("geiger")
library("stringr")
text.string<-trees[1,2]
text.string<-gsub("\\]", "#", gsub("\\[", "#", text.string))
vert.tree<-read.tree(text=text.string)

plotTree(vert.tree, ftype="i")
#tiplabels(vert.tree$tip.label)
nodelabels(vert.tree$node.label)
