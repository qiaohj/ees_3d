library("dplyr")
library("DBI")
library(ggplot2)
library(Rmisc)
base<-"/media/huijieqiao/Butterfly/SMART_SPECIES"

simulations<-data.frame(label=c("12337_POOR_NARROW_1_1", 
                                "12337_POOR_NARROW_2_0.01", "12337_POOR_NARROW_2_0.1", 
                                "12337_POOR_NARROW_3_0.01", "12337_POOR_NARROW_3_0.1", 
                                "12337_POOR_NARROW_4_0.01", "12337_POOR_NARROW_4_0.1",
                                "12337_POOR_NARROW_5_0.01", "12337_POOR_NARROW_5_0.1"),
                        nb="NARROW",
                        da="POOR",
                        global_id=12337,
                        niche_envolution_individual_ratio=c(1, rep(c(0.01, 0.1), 4)),
                        stringsAsFactors = F)
result<-data.frame()
for (i in c(1:nrow(simulations))){
  s<-simulations[i,]


  log<-sprintf("%s/RESULTS/%s/%s.log", base, s$label, s$label)
  if (!file.exists(log)){
    log<-sprintf("%s/RESULTS_TEST/%s/%s.log", base, s$label, s$label)
  }
  
  print(paste(i, nrow(simulations), s$label))
  log_db<-read.table(log, head=F, sep=",", stringsAsFactors = F)
  colnames(log_db)<-c("Y", "ID", "GROUP", "N", "SP_ID", "SUITABLE")
  if ((nrow(log_db)>10000)&(length(unique(log_db$SP_ID))>10)){
    #asdf
  }
  log_db$SP_ID<-as.character(log_db$SP_ID)
  log_db<-as_tibble(log_db)
  
  individual_ratio<-log_db %>% dplyr::group_by(Y, SUITABLE) %>% dplyr::summarise(N_IND=sum(N))
  sp_N<-log_db %>% dplyr::group_by(Y) %>% dplyr::summarise(N_SP=length(unique(SP_ID)))
  cell_N<-log_db %>% dplyr::group_by(Y, SUITABLE) %>% dplyr::count()
  colnames(cell_N)[3]<-"N_CELL"
  item<-inner_join(individual_ratio, sp_N, by="Y")
  item<-inner_join(item, cell_N, by=c("Y", "SUITABLE"))
  item$LABLE<-s$label
  item$NB<-s$nb
  item$DA<-s$da
  ff<-strsplit(s$label, "_")[[1]]
  item$EVO_TYPE<-as.numeric(ff[4])
  item$GLOBAL_ID<-s$global_id
  item$EVO_RATIO<-s$niche_envolution_individual_ratio
  if (nrow(result)==0){
    result<-item
  }else{
    result<-bind_rows(result, item)
  }
}
result$Y<-result$Y*-1
result$AVERAGE_N_CELL<-result$N_CELL/result$N_SP

mean_df<-result %>%
  dplyr::group_by(Y, SUITABLE, NB, DA, EVO_RATIO, EVO_TYPE) %>%
  dplyr::summarize(Mean_N_IND = mean(N_IND, na.rm=TRUE),
                   Mean_N_SP = mean(N_SP, na.rm=TRUE),
                   Mean_N_CELL = mean(N_CELL, na.rm=TRUE),
                   Mean_AVERAGE_N_CELL = mean(AVERAGE_N_CELL, na.rm=TRUE),
                   SD_N_IND = sd(N_IND),
                   SD_N_SP = sd(N_SP),
                   SD_N_CELL = sd(N_CELL),
                   SD_AVERAGE_N_CELL = sd(AVERAGE_N_CELL),
                   Median_N_IND = quantile(N_IND, na.rm=TRUE, .5),
                   Median_N_SP = quantile(N_SP, na.rm=TRUE, .5),
                   Median_N_CELL = quantile(N_CELL, na.rm=TRUE, .5),
                   Median_AVERAGE_N_CELL = quantile(AVERAGE_N_CELL, na.rm=TRUE, .5),
                   CI_N_IND = CI(N_IND, ci=0.95)[2] - CI(N_IND, ci=0.95)[3],
                   CI_N_SP = CI(N_SP, ci=0.95)[2] - CI(N_SP, ci=0.95)[3],
                   CI_N_CELL = CI(N_CELL, ci=0.95)[2] - CI(N_CELL, ci=0.95)[3],
                   CI_AVERAGE_N_CELL = CI(AVERAGE_N_CELL, ci=0.95)[2] - CI(AVERAGE_N_CELL, ci=0.95)[3]
  )
mean_df[is.na(mean_df)]<-0
mean_df$label<-paste(mean_df$NB, mean_df$DA, mean_df$EVO_RATIO, mean_df$EVO_TYPE)

mean_df_ratio<-result %>%
  dplyr::group_by(Y, SUITABLE, NB, DA, EVO_RATIO, EVO_TYPE, LABLE, GLOBAL_ID) %>%
  dplyr::summarize(N_IND = sum(N_IND, na.rm=TRUE),
                   N_SP = sum(N_SP, na.rm=TRUE),
                   N_CELL = sum(N_CELL, na.rm=TRUE)
  )
item_0<-mean_df_ratio %>% filter(SUITABLE==0)
item_1<-mean_df_ratio %>% filter(SUITABLE==1)
mean_df_ratio_merge<-full_join(item_0, item_1, by=c("Y", "NB", "DA", "EVO_RATIO", "EVO_TYPE", "LABLE", "GLOBAL_ID"))
mean_df_ratio_merge[is.na(mean_df_ratio_merge)]<-0
mean_df_ratio_merge$ratio<-mean_df_ratio_merge$N_IND.x/(mean_df_ratio_merge$N_IND.x+mean_df_ratio_merge$N_IND.y)
mean_df_ratio_df<-mean_df_ratio_merge %>%
  dplyr::group_by(Y, NB, DA, EVO_RATIO, EVO_TYPE) %>%
  dplyr::summarize(Mean_ratio = mean(ratio, na.rm=TRUE),
                   SD_ratio = sd(ratio),
                   Median_ratio = quantile(ratio, na.rm=TRUE, .5),
                   CI_ratio = CI(ratio, ci=0.95)[2] - CI(ratio, ci=0.95)[3]
  )
mean_df_ratio_df[is.na(mean_df_ratio_df)]<-0
mean_df_ratio_df$label<-paste(mean_df_ratio_df$NB, mean_df_ratio_df$DA, mean_df_ratio_df$EVO_RATIO, mean_df_ratio_df$EVO_TYPE)

comb<-expand.grid(unique(mean_df$NB), unique(mean_df$NB), stringsAsFactors=F)
cols<-rep(c("deepskyblue", "deepskyblue3", "black", 
            "khaki1", "khaki3", "black", 
            "hotpink", "hotpink3", "black", 
            "darkolivegreen3", "darkolivegreen", "black",
            "black", "black", "black"), nrow(comb))
comb<-expand.grid(unique(mean_df$EVO_RATIO), unique(mean_df$EVO_TYPE), unique(mean_df$NB), unique(mean_df$DA), stringsAsFactors=F)
comb$label<-paste(comb$Var3, comb$Var4, comb$Var1, comb$Var2)
names(cols)<-comb$label

ggplot(mean_df %>% filter(SUITABLE==1), 
       aes(x=Y, y=Mean_N_SP))+
  geom_line(aes(color=factor(label)))+
  geom_ribbon(aes(ymin=Mean_N_SP-CI_N_SP, ymax=Mean_N_SP+CI_N_SP, fill=factor(label)), color=NA, alpha=0.3)+
  theme_bw()+
  scale_colour_manual(values = cols,aesthetics = c("colour", "fill"))

ggplot(mean_df %>% filter(SUITABLE==1), 
       aes(x=Y, y=Mean_N_CELL))+
  geom_line(aes(color=factor(label)))+
  geom_ribbon(aes(ymin=Mean_N_CELL-CI_N_CELL, ymax=Mean_N_CELL+CI_N_CELL, fill=factor(label)), color=NA, alpha=0.3)+
  theme_bw()+
  scale_colour_manual(values = cols,aesthetics = c("colour", "fill"))

ggplot(mean_df %>% filter((SUITABLE==1)&(Y<=-1100)), 
       aes(x=Y, y=Mean_N_CELL))+
  geom_line(aes(color=factor(label)))+
  geom_ribbon(aes(ymin=Mean_N_CELL-CI_N_CELL, ymax=Mean_N_CELL+CI_N_CELL, fill=factor(label)), color=NA, alpha=0.3)+
  theme_bw()+
  scale_colour_manual(values = cols,aesthetics = c("colour", "fill"))

ggplot(mean_df %>% filter(SUITABLE==1), 
       aes(x=Y, y=Mean_AVERAGE_N_CELL))+
  geom_line(aes(color=factor(label)))+
  geom_ribbon(aes(ymin=Mean_AVERAGE_N_CELL-CI_AVERAGE_N_CELL, ymax=Mean_AVERAGE_N_CELL+CI_AVERAGE_N_CELL, fill=factor(label)), color=NA, alpha=0.3)+
  theme_bw()+
  scale_colour_manual(values = cols,aesthetics = c("colour", "fill"))

ggplot(mean_df_ratio_df, aes(x=Y, y=Mean_ratio))+
  geom_line(aes(color=factor(label)))+
  geom_ribbon(aes(ymin=Mean_ratio-CI_ratio, ymax=Mean_ratio+CI_ratio, fill=factor(label)), color=NA, alpha=0.3)+
  theme_bw()+
  scale_colour_manual(values = cols,aesthetics = c("colour", "fill"))+
  ylim(0, 0.4)

ggplot(mean_df_ratio_df %>% filter((Y<=-1100)&(Y>=-1198)), aes(x=Y, y=Mean_ratio))+
  geom_line(aes(color=factor(label)))+
  geom_ribbon(aes(ymin=Mean_ratio-CI_ratio, ymax=Mean_ratio+CI_ratio, fill=factor(label)), color=NA, alpha=0.3)+
  theme_bw()+
  scale_colour_manual(values = cols,aesthetics = c("colour", "fill"))



nb<-read.table(sprintf("%s/RESULTS_TEST/%s/%s.nb.log", base, "12337_POOR_NARROW_5_0.1", "12337_POOR_NARROW_5_0.1"), 
               head=F, sep=",", stringsAsFactors = F)


head(nb)
tail(nb)
nb$p1<-nb$V18-nb$V17
nb$p2<-nb$V19-nb$V18


plot(nb$V1, nb$p1)
plot(nb$V1, nb$p2)

hist(nb$V17)
hist(nb$V18)
hist(nb$V19)

label<-"12337_POOR_NARROW_5_0.01"
label<-"14823_GOOD_MODERATE_5_0.01"
nb_4<-read.csv(sprintf("%s/RESULTS_TEST/%s/%s.nb_4.log", base, label, label), 
               head=F, sep=",", stringsAsFactors = F)

nb<-read.csv(sprintf("%s/RESULTS_TEST/%s/%s.nb.log", base, label, label), 
               head=F, sep=",", stringsAsFactors = F)
head(nb)
tail(nb)

head(nb_4[which(nb_4$V6!=0.25),], 100)

tail(nb_4[which(nb_4$V6!=0.25),], 100)

head(nb_4[which((nb_4$V3==10960)),], 20)
head(nb_4[which((nb_4$V1==55)&(nb_4$V3==10960)),], 100)

head(nb_4[which((nb_4$V6>0.25)),], 20)

N_Cell<-data.frame(table(nb_4$V3))
N_Cell[which(N_Cell$Freq==max(N_Cell$Freq)),]
nb_4_sub<-nb_4[which(nb_4$V3==10960),]
nb_sub<-nb[which(nb$V2==10960),]

nb_4_sub<-nb_4[which(nb_4$V3==12172),]

ggplot(nb_4_sub[which(nb_4_sub$V8==1),]) + geom_point(aes(x=V1, y=V6, color=factor(V4)))


mean_df<-result %>%
  dplyr::group_by(Y, SUITABLE, NB, DA, EVO_RATIO, EVO_TYPE) %>%
  dplyr::summarize(Mean_N_IND = mean(N_IND, na.rm=TRUE),
                   Mean_N_SP = mean(N_SP, na.rm=TRUE),
                   Mean_N_CELL = mean(N_CELL, na.rm=TRUE),
                   Mean_AVERAGE_N_CELL = mean(AVERAGE_N_CELL, na.rm=TRUE),
                   SD_N_IND = sd(N_IND),
                   SD_N_SP = sd(N_SP),
                   SD_N_CELL = sd(N_CELL),
                   SD_AVERAGE_N_CELL = sd(AVERAGE_N_CELL),
                   Median_N_IND = quantile(N_IND, na.rm=TRUE, .5),
                   Median_N_SP = quantile(N_SP, na.rm=TRUE, .5),
                   Median_N_CELL = quantile(N_CELL, na.rm=TRUE, .5),
                   Median_AVERAGE_N_CELL = quantile(AVERAGE_N_CELL, na.rm=TRUE, .5),
                   CI_N_IND = CI(N_IND, ci=0.95)[2] - CI(N_IND, ci=0.95)[3],
                   CI_N_SP = CI(N_SP, ci=0.95)[2] - CI(N_SP, ci=0.95)[3],
                   CI_N_CELL = CI(N_CELL, ci=0.95)[2] - CI(N_CELL, ci=0.95)[3],
                   CI_AVERAGE_N_CELL = CI(AVERAGE_N_CELL, ci=0.95)[2] - CI(AVERAGE_N_CELL, ci=0.95)[3]
  )
