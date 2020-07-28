

library("dplyr")
library("DBI")
library("phytools")
library("tidyr")
library("data.table")
library("tibble")
library("raster")
library("sp")
library("ggplot2")
library("sgeostat")



base<-"/home/huijieqiao/git/ees_3d_data/niche_conservatism"
db_base<-"/home/huijieqiao/git/ees_3d_data/SMART_SPECIES"
base2<-"/media/huijieqiao/Butterfly/SMART_SPECIES"

rdas<-list.files(sprintf("%s/Data/items_distribution", base), pattern="\\.rda", full.names = T)

distribution<-NULL

i=1
bind<-function(df1, df2){
  if (is.null(df1)){
    df1<-df2
  }else{
    df1<-bind_rows(df1, df2)
  }
  return(df1)
}

#df_N_SP_all<-NULL
for (i in c(1:length(rdas))){
  print(paste(i, length(rdas), rdas[i]))
  target<-sprintf("%s/Data/item_richness/df_N_SP_%d.rda", base, i)
  if (file.exists(target)){
    next()
  }
  saveRDS(NA, target)
  f<-rdas[i]
  df<-readRDS(f)
  if (is.null(df)){
    next()
  }
  colnames(df)
  df_N_SP<-df%>%
    dplyr::group_by(Y, lon,lat, MIN_TEMP, MAX_TEMP, MAX_PREC, ID, da, nb, evo_type, evo_ratio)%>%
    dplyr::summarise(N_SP=n())
  saveRDS(df_N_SP, target)
  #df_N_SP_all<-bind(df_N_SP_all, df_N_SP)
}

