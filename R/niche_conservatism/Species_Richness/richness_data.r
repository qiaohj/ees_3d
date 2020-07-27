library(dplyr)
library(ggplot2)
library(raster)
base<-"/home/huijieqiao/git/ees_3d_data/niche_conservatism"
db_base<-"/home/huijieqiao/git/ees_3d_data/SMART_SPECIES"
base2<-"/media/huijieqiao/Butterfly/SMART_SPECIES"
t_f<-sprintf("%s/Data/distribution.rda", base)
distribution<-readRDS(t_f)
length(unique(distribution$global_id))
dim(distribution)
stat<-readRDS(sprintf("%s/Data/stat.rda", base))

length(unique(stat[]$global_id))
length(unique(distribution$global_id))
unique(distribution$nb)

stat[which(stat$n_extinction==0)&(!(stat$global_id %in% distribution$global_id)),]

distribution%>%filter(global_id==12246)
