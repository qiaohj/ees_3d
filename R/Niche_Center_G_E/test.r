


df<-read.csv("/media/huijieqiao/Butterfly/Niche_Center_E_G/Results/4365_POOR_BROAD_7_0.05_935/4365_POOR_BROAD_7_0.05_935.log", head=F)
colnames(df)<-c("Y", "GLOBAL_ID", "GROUP_ID", "N", "SP_ID", "IS_SUITABLE")
df2<-read.csv("/media/huijieqiao/QNAS/Speciation_Extinction/Results/4365_POOR_BROAD_1_1_935/4365_POOR_BROAD_1_1_935.log", head=F)
colnames(df2)<-c("Y", "GLOBAL_ID", "GROUP_ID", "N", "SP_ID", "IS_SUITABLE")

y1<-data.frame(table(df$Y))
y1$type="T1"
y2<-data.frame(table(df2$Y))
y2$type="T2"
y_df<-rbind(y1, y2)
library(ggplot2)
y_df$Var1<-as.numeric(as.character(y_df$Var1))
ggplot(y_df, aes(x=Var1, y=Freq, color=factor(type)))+geom_line()


library(ggplot2)
library(dplyr)
library(RSQLite)
library(DBI)
library("ape")
library("phangorn")
library("phytools")
library("geiger")
library("stringr")
library(tidyverse)
library(plotKML)

logdb<-"/media/huijieqiao/Butterfly/Niche_Center_E_G/Results/4365_POOR_BROAD_7_0.05_935/4365_POOR_BROAD_7_0.05_935.sqlite"
mydb <- dbConnect(RSQLite::SQLite(), logdb)
trees<-dbReadTable(mydb, "trees")
suitable<-dbReadTable(mydb, "suitable")
dbDisconnect(mydb)
text.string<-trees[1,2]
text.string<-gsub("\\]", "#", gsub("\\[", "#", text.string))
vert.tree<-read.tree(text=text.string)

plotTree(vert.tree, ftype="i")
#tiplabels(vert.tree$tip.label)
nodelabels(vert.tree$node.label)

