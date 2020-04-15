library("dplyr")
library("DBI")
base<-"/home/huijieqiao/git/ees_3d_data/SMART_SPECIES"
mydb <- dbConnect(RSQLite::SQLite(), sprintf("%s/conf.sqlite", base))  
simulations<-dbReadTable(mydb, "simulations")
dbDisconnect(mydb) 
simulations<-simulations %>% filter(nb!="BROAD")
simulations<-simulations %>% filter(is_run==1)
i=6
result<-readRDS(sprintf("%s/Tables/individual_ratio.rda", base))
print(result)
finished<-unique(result$LABLE)
for (i in c(1:nrow(simulations))){
  s<-simulations[i,]
  if (s$label %in% finished){
    next()
  }
  finished<-c(finished, s$label)
  log<-sprintf("%s/Results/%s/%s.log", base, s$label, s$label)
  if (!file.exists(log)){
    next()
  }
  
  print(paste(i, nrow(simulations), s$label))
  log_db<-read.table(log, head=F, sep=",", stringsAsFactors = F)
  colnames(log_db)<-c("Y", "ID", "GROUP", "N", "SP_ID", "SUITABLE")
  if ((nrow(log_db)>10000)&(length(unique(log_db$SP_ID))>10)){
    #asdf
  }
  log_db$SP_ID<-as.character(log_db$SP_ID)
  log_db<-as_tibble(log_db)
  
  individual_ratio<-log_db %>% group_by(Y, SUITABLE) %>% summarise(N_IND=sum(N))
  sp_N<-log_db %>% group_by(Y) %>% summarise(N_SP=length(unique(SP_ID)))
  cell_N<-log_db %>% group_by(Y, SUITABLE) %>% count()
  colnames(cell_N)[3]<-"N_CELL"
  item<-inner_join(individual_ratio, sp_N, by="Y")
  item<-inner_join(item, cell_N, by=c("Y", "SUITABLE"))
  item$LABLE<-s$label
  item$NB<-s$nb
  item$DA<-s$da
  item$GLOBAL_ID<-s$global_id
  item$EVO_RATIO<-s$niche_envolution_individual_ratio
  if (nrow(result)==0){
    result<-item
  }else{
    result<-bind_rows(result, item)
  }
}
saveRDS(result, sprintf("%s/Tables/individual_ratio.rda", base))
