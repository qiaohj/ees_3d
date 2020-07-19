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


#the rda file was generated by global_id_lonlat.r
mask_df<-readRDS(sprintf("%s/Data/ENV/mask_df.rda", base))

simulations<-NULL
for (i in c(1,2,7)){
  print(i)
  mydb <- dbConnect(RSQLite::SQLite(), sprintf("%s/conf_%s.sqlite", db_base, i))  
  simulation<-dbReadTable(mydb, "simulations")
  dbDisconnect(mydb) 
  if (is.null(simulations)){
    simulations<-simulation
  }else{
    simulations<-bind_rows(simulation, simulations)
  }
}
handle_label<-function(label){
  if (length(label)==4){
    label<-c(label, label[4])
  }
  label[c(1,4,5)]
}
simulations<-simulations %>% filter(nb!="BROAD")
simulations<-simulations %>% filter(is_run==1)
cmd<-c()
i=15234

getID<-function(label){
  gsub("SP", "", strsplit(label, " ")[[1]][1])
}
getSPID<-function(tree, node){
  if (is.null(node)){
    return("")
  }
  if (node>Ntip(tree)){
    label_index<-node - Ntip(tree)
    node_type<-"node"
    label<-tree$node.label[label_index]
  }else{
    label_index<-node
    node_type<-"tip"
    label<-tree$tip.label[label_index]
  }
  label<-getID(label)
  parent<-getParent(tree, node)
  if (is.null(parent)){
    return(getID(tree$node.label[node - Ntip(tree)]))
  }else{
    return(sprintf("%s-%s", getSPID(tree, parent),
                   label))
  }
}
node<-Ntip(tree)+3
getSPID(tree, Ntip(tree)+3)
getSPID(tree, Ntip(tree))
getSPID(tree, 1)
start=1808
if (start==1){
  stat<-NULL
  detail<-NULL
  speciation_df<-NULL
  extinction_df<-NULL
  sp_character<-NULL
}else{
  stat<-readRDS(sprintf("%s/Data/stat.rda", base))
  detail<-readRDS(sprintf("%s/Data/detail.rda", base))
  sp_character<-readRDS(sprintf("%s/Data/sp_character.rda", base))
  extinction_df<-readRDS(sprintf("%s/Data/extinction_df.rda", base))
  speciation_df<-readRDS(sprintf("%s/Data/speciation_df.rda", base))
  stat<-stat%>%dplyr::filter(global_id!=start)
  detail<-detail%>%dplyr::filter(global_id!=start)
  sp_character<-sp_character%>%dplyr::filter(global_id!=start)
  extinction_df<-extinction_df%>%dplyr::filter(ID!=start)
  speciation_df<-speciation_df%>%dplyr::filter(global_id!=start)
}
for (i in c(start:nrow(simulations))){
  print(paste(i, nrow(simulations)))
  s<-simulations[i,]
  nb<-strsplit(s$nb_v, "\\|")[[1]]
  nb_temp<-as.numeric(strsplit(nb[1], ",")[[1]])
  nb_prec<-as.numeric(strsplit(nb[3], ",")[[1]])
  log<-sprintf("%s/RESULTS/%s/%s.log.env.rda", base2, s$label, s$label)
  log_df<-readRDS(log)
  
  log_db<-sprintf("%s/RESULTS/%s/%s.sqlite", base2, s$label, s$label)
  mydb <- dbConnect(RSQLite::SQLite(), log_db)  
  trees<-dbReadTable(mydb, "trees")
  dbDisconnect(mydb)
  
  text.string<-trees[which(trees$TYPE=="NEWICK"), "CONTENT"]
  if (length(text.string)==0){
    text.string<-sprintf("SP%d @ 1200-1199:1;", s$global_id)
  }
  text.string<-gsub("\\]", "#", gsub("\\[", "#", text.string))
  if (!grepl("\\(", text.string)){
    text.string<-paste("()", text.string, sep="")
  }
  tree<-read.newick(text=text.string)
  
  #plotTree(tree)
  #nodelabels(text=tree$node.label)
  
  lll<-strsplit(tree$node.label, "[-\\@\\ ]")
  node_labels<-data.frame(stringsAsFactors = F,
                          matrix(unlist(purrr::map(lll, .f = ~ handle_label(.x))), ncol=3, byrow=T)
  )
  
  
  colnames(node_labels)<-c("sp_label", "from", "to")
  node_labels$from<-as.numeric(node_labels$from)
  node_labels$to<-as.numeric(node_labels$to)
  node_labels$type<-"NODE"
  node_labels$index<-Ntip(tree) +as.numeric(rownames(node_labels))
  
  
  if (length(tree$tip.label)>0){
    lll<-strsplit(tree$tip.label, "[-\\@\\ ]")
    tip_labels<-data.frame(stringsAsFactors = F,
                           matrix(unlist(purrr::map(lll, .f = ~ handle_label(.x))), ncol=3, byrow=T)
    )
    colnames(tip_labels)<-c("sp_label", "from", "to")
    tip_labels$from<-as.numeric(tip_labels$from)
    tip_labels$to<-as.numeric(tip_labels$to)
    tip_labels$type<-"LEAF"
    tip_labels$index<-as.numeric(rownames(tip_labels))
    node_labels<-bind_rows(tip_labels, node_labels)
  }else{
    node_labels$type<-"LEAF"
  }
  
  node_labels<-node_labels %>% 
    rowwise() %>% 
    mutate(SP=getSPID(tree, index),
           PARENT=getSPID(tree, getParent(tree, index)))
  if (min(node_labels$to)!=min(log_df$Y)){
    aasdf
  }
  
  n_speciation<-nrow(node_labels%>%dplyr::filter(type=="NODE"))
  n_extinction<-nrow(node_labels%>%dplyr::filter((type=="LEAF")&(to!=0)))
  
  s<-s[, c("global_id", "da", "nb", "evo_type", "niche_envolution_individual_ratio")]
  s$n_speciation<-n_speciation
  s$n_extinction<-n_extinction
  
  node_labels$global_id<-s$global_id
  node_labels$da<-s$da
  node_labels$nb<-s$nb
  node_labels$evo_type<-s$evo_type
  node_labels$evo_ratio<-s$niche_envolution_individual_ratio
  
  if (is.null(stat)){
    stat<-s
    detail<-node_labels
  }else{
    stat<-bind_rows(stat, s)
    detail<-bind_rows(detail, node_labels)
  }
  #Area, Niche Breadth, Lat, Lon Range
  dis<-log_df%>%dplyr::filter(SUITABLE==1)
  dis_n<-dis%>%dplyr::group_by(Y, SP_ID)%>%
    dplyr::summarise(AREA=n(),
                     MIN_TEMP=min(MIN_TEMP),
                     MAX_MIN_TEMP=max(MIN_TEMP),
                     MIN_MAX_TEMP=min(MAX_TEMP),
                     MAX_TEMP=max(MAX_TEMP),
                     MIN_PREC=min(MAX_PREC),
                     MAX_PREC=max(MAX_PREC),
                     MIN_LAT=min(lat),
                     MAX_LAT=max(lat),
                     MIN_LON=min(lon),
                     MAX_LON=max(lon))
  dis_n$RANG_TEMP<-dis_n$MAX_TEMP-dis_n$MIN_TEMP
  dis_n$RANG_PREC<-dis_n$MAX_PREC-dis_n$MIN_PREC
  dis_n$RANG_LON<-dis_n$MAX_LON-dis_n$MIN_LON
  dis_n$RANG_LAT<-dis_n$MAX_LAT-dis_n$MIN_LAT
  dis_n$global_id<-s$global_id
  dis_n$da<-s$da
  dis_n$nb<-s$nb
  dis_n$evo_type<-s$evo_type
  dis_n$evo_ratio<-s$niche_envolution_individual_ratio
  if (is.null(sp_character)){
    sp_character<-dis_n
  }else{
    sp_character<-bind_rows(sp_character, dis_n)
  }
  for (j in c(1:nrow(node_labels))){
    
    node<-node_labels[j,]
    
    #Speciation
    if (node$type=="NODE"){
      print(paste(j, nrow(node_labels), "SPECIATION"))
      item<-node_labels%>%dplyr::filter(PARENT==node$SP)
      dis<-log_df%>%dplyr::filter((SUITABLE==1)&(Y==node$to)&(SP_ID %in% item$SP))
      ch<-chull(dis %>% dplyr::select(lon, lat))
      
      range_lon<-range(dis$lon)
      range_lat<-range(dis$lat)
      potential_ba<-mask_df%>%dplyr::filter((Y==node$to)&
                                              between(lon, range_lon[1], range_lon[2])&
                                              between(lat, range_lat[1], range_lat[2]))
      potential_ba$in_out<-in.chull(potential_ba$lon, potential_ba$lat,
                                pull(dis[ch, "lon"]), pull(dis[ch, "lat"]))
      
      potential_ba<-potential_ba%>%dplyr::filter((in_out==T)&(!(global_id%in%dis$ID)))
      if (nrow(potential_ba)==0){
        next()
      }
      
      potential_ba$MIN_TEMP_IN<-between(potential_ba$MIN_TEMP, nb_temp[1], nb_temp[2])
      potential_ba$MAX_TEMP_IN<-between(potential_ba$MAX_TEMP, nb_temp[1], nb_temp[2])
      potential_ba$MAX_PREC_IN<-between(potential_ba$MAX_PREC, nb_temp[1], nb_temp[2])
      potential_ba$global_id<-s$global_id
      potential_ba$da<-s$da
      potential_ba$nb<-s$nb
      potential_ba$evo_type<-s$evo_type
      potential_ba$evo_ratio<-s$niche_envolution_individual_ratio
      if(F){
        ggplot(data=dis, aes(x=lon, y=lat))+
          geom_point(data=potential_ba, aes(color=factor(in_out)))+
          geom_path(data=dis[c(ch, ch[1]),], aes(x=lon, y=lat))+
          geom_point(data=dis, aes(color=factor(SP_ID)))
      }
      if (is.null(speciation_df)){
        speciation_df<-potential_ba
      }else{
        speciation_df<-bind_rows(speciation_df, potential_ba)
      }
    }
    #Extinction
    if ((node$type=="LEAF")&(node$to!=0)){
      print(paste(j, nrow(node_labels), "EXTINCTION"))
      dis<-log_df%>%dplyr::filter((Y==node$to)&(SP_ID==node$SP))
      dis$MIN_TEMP_IN<-between(dis$MIN_TEMP, nb_temp[1], nb_temp[2])
      dis$MAX_TEMP_IN<-between(dis$MAX_TEMP, nb_temp[1], nb_temp[2])
      dis$MAX_PREC_IN<-between(dis$MAX_PREC, nb_temp[1], nb_temp[2])
      
      dis$global_id<-s$global_id
      dis$da<-s$da
      dis$nb<-s$nb
      dis$evo_type<-s$evo_type
      dis$evo_ratio<-s$niche_envolution_individual_ratio
      if (is.null(extinction_df)){
        extinction_df<-dis
      }else{
        extinction_df<-bind_rows(extinction_df, dis)
      }
    }
  }
  
  #saveRDS(stat, sprintf("%s/Data/stat.rda", base))
  #saveRDS(detail, sprintf("%s/Data/detail.rda", base))
  #saveRDS(sp_character, sprintf("%s/Data/sp_character.rda", base))
  #saveRDS(extinction_df, sprintf("%s/Data/extinction_df.rda", base))
  #saveRDS(speciation_df, sprintf("%s/Data/speciation_df.rda", base))
  
  
}

colnames(stat)[5]<-"evo_ratio"
stat[which(stat$evo_ratio==0.01), "evo_ratio"]<-0.005
stat[which(stat$evo_ratio==0.1), "evo_ratio"]<-0.05
detail[which(detail$evo_ratio==0.01), "evo_ratio"]<-0.005
detail[which(detail$evo_ratio==0.1), "evo_ratio"]<-0.05
sp_character[which(sp_character$evo_ratio==0.01), "evo_ratio"]<-0.005
sp_character[which(sp_character$evo_ratio==0.1), "evo_ratio"]<-0.05
extinction_df[which(extinction_df$evo_ratio==0.01), "evo_ratio"]<-0.005
extinction_df[which(extinction_df$evo_ratio==0.1), "evo_ratio"]<-0.05
speciation_df[which(speciation_df$evo_ratio==0.01), "evo_ratio"]<-0.005
speciation_df[which(speciation_df$evo_ratio==0.1), "evo_ratio"]<-0.05

saveRDS(stat, sprintf("%s/Data/stat.rda", base))
saveRDS(detail, sprintf("%s/Data/detail.rda", base))
saveRDS(sp_character, sprintf("%s/Data/sp_character.rda", base))
saveRDS(extinction_df, sprintf("%s/Data/extinction_df.rda", base))
saveRDS(speciation_df, sprintf("%s/Data/speciation_df.rda", base))
