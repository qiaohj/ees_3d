library(dplyr)
library(data.table)
base<-"/home/huijieqiao/git/ees_3d_data/niche_conservatism"
i=75
richness<-NULL
for (i in c(1:200)){
  print(paste(Sys.time(), i, 200))
  target<-sprintf("%s/Data/item_richness_rep/df_richness_%d.rda", base, i)
  print(paste(Sys.time(), "Reading Data"))
  df<-readRDS(target)
  nrow<-nrow(df)
  if (is.null(nrow)){
    nrow<-0
  }
  if (nrow==0){
    next()
  }
  
  if (is.null(richness)){
    richness<-df
  }else{
    
    system.time({
      if (class(richness)[1]!="data.table"){
        print(paste(Sys.time(), "Converting richness data type"))
        richness<-data.table(richness)
      }
      #print(paste(Sys.time(), "Setting richness keys"))
      #setkeyv(richness, c("Y", "GLOBAL_ID", "LON", "LAT", 
      #                    "MIN_TEMP", "MAX_TEMP", "MAX_PREC",
      #                    "DA", "NB", "EVO_TYPE", "EVO_RATIO",
      #                    "REP"))
      print(paste(Sys.time(), "Converting df data type"))
      df<-data.table(df)
      #print(paste(Sys.time(), "Setting df keys"))
      #setkeyv(df, c("Y", "GLOBAL_ID", "LON", "LAT", 
      #              "MIN_TEMP", "MAX_TEMP", "MAX_PREC",
      #              "DA", "NB", "EVO_TYPE", "EVO_RATIO",
      #              "REP"))
      print(paste(Sys.time(), "Merging richness and df"))
      richness_t<-merge(richness, df, 
                                  by=c("Y", "GLOBAL_ID", "LON", "LAT", 
                                       "MIN_TEMP", "MAX_TEMP", "MAX_PREC",
                                       "DA", "NB", "EVO_TYPE", "EVO_RATIO",
                                       "REP"), all=T)
      print(paste(Sys.time(), "Fixing NA values"))
      richness_t[which(is.na(richness_t$N_SP.x)), "N_SP.x"]<-0
      richness_t[which(is.na(richness_t$N_SP.y)), "N_SP.y"]<-0
      richness_t$N_SP<-richness_t$N_SP.x+richness_t$N_SP.y
      print(paste(Sys.time(), "Generating new richness"))
      richness<-richness_t[, c("Y", "GLOBAL_ID", "LON", "LAT", 
                               "MIN_TEMP", "MAX_TEMP", "MAX_PREC",
                               "DA", "NB", "EVO_TYPE", "EVO_RATIO",
                               "REP", "N_SP")]
      print(paste(Sys.time(), "nrow(richness):", nrow(richness)))
      
    })
    if (F){
      system.time({
        richness_t<-full_join(richness, df, 
                        by=c("Y", "GLOBAL_ID", "LON", "LAT", 
                             "MIN_TEMP", "MAX_TEMP", "MAX_PREC",
                             "DA", "NB", "EVO_TYPE", "EVO_RATIO",
                             "REP"))
    })}

  }
}
saveRDS(richness, sprintf("%s/Data/richness.rda", base, i))
