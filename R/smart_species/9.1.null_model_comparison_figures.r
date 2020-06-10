library(dplyr)
library(stringr)
library(ggplot2)
library(Rmisc)
setwd("~/git/ees_3d/R/smart_species")
base<-"/home/huijieqiao/git/ees_3d_data/SMART_SPECIES"

count_N_IND_DIFF<-readRDS( "../../../ees_3d_data/SMART_SPECIES/Tables/9.null_model_comparison_analysis/count_N_IND_DIFF.rda")
count_N_SP_DIFF<-readRDS("../../../ees_3d_data/SMART_SPECIES/Tables/9.null_model_comparison_analysis/count_N_SP_DIFF.rda")
count_N_CELL_DIFF<-readRDS("../../../ees_3d_data/SMART_SPECIES/Tables/9.null_model_comparison_analysis/count_N_CELL_DIFF.rda")
sort_N_CELL<-readRDS("../../../ees_3d_data/SMART_SPECIES/Tables/9.null_model_comparison_analysis/sort_N_CELL.rda")
mean_sort_N_CELL<-readRDS("../../../ees_3d_data/SMART_SPECIES/Tables/9.null_model_comparison_analysis/mean_sort_N_CELL.rda")
sort_AVERAGE_N_CELL<-readRDS("../../../ees_3d_data/SMART_SPECIES/Tables/9.null_model_comparison_analysis/sort_AVERAGE_N_CELL.rda")
mean_sort_AVERAGE_N_CELL<-readRDS("../../../ees_3d_data/SMART_SPECIES/Tables/9.null_model_comparison_analysis/mean_sort_AVERAGE_N_CELL.rda")
sort_N_SP<-readRDS("../../../ees_3d_data/SMART_SPECIES/Tables/9.null_model_comparison_analysis/sort_N_SP.rda")
mean_sort_N_SP<-readRDS("../../../ees_3d_data/SMART_SPECIES/Tables/9.null_model_comparison_analysis/mean_sort_N_SP.rda")

comb<-expand.grid(unique(count_N_IND_DIFF$NB), unique(count_N_IND_DIFF$DA), stringsAsFactors=F)
cols<-rep(c("red", "red4", "black", 
            "orange", "orange4", "black", 
            "yellow", "yellow4", "black", 
            "green", "green4", "black",
            "turquoise", "turquoise4", "black",
            "blue", "blue4", "black"), nrow(comb))
comb<-expand.grid(unique(count_N_IND_DIFF$EVO_RATIO), c(2:7), 
                  unique(count_N_IND_DIFF$NB), unique(count_N_IND_DIFF$DA), stringsAsFactors=F)
comb$label<-paste(comb$Var3, comb$Var4, comb$Var1, comb$Var2)
names(cols)<-comb$label

comb<-expand.grid(unique(count_N_IND_DIFF$NB), unique(count_N_IND_DIFF$DA), stringsAsFactors=F)
i=1
for (i in c(1:nrow(comb))){
  com<-comb[i,]
  count_N_IND_DIFF_item<-count_N_IND_DIFF %>% filter(NB==com$Var1 & DA==com$Var2)
  count_N_IND_DIFF_item <- count_N_IND_DIFF_item %>% filter(SUITABLE==1)
  count_N_IND_DIFF_item <- count_N_IND_DIFF_item %>% filter(EVO_TYPE!=1)
  count_N_IND_DIFF_item <- count_N_IND_DIFF_item %>% filter(EVO_RATIO!=0.1)
  #count_N_IND_DIFF_item <- count_N_IND_DIFF_item %>% filter(N_IND_DIFF==-1)
  #count_N_IND_DIFF_item <- count_N_IND_DIFF_item %>% filter(N_IND_DIFF!=0)
  
  test<-count_N_IND_DIFF_item %>% filter((EVO_TYPE==2)&(EVO_RATIO==0.01))
  table(test$N_IND_DIFF)
  p<-ggplot(count_N_IND_DIFF_item, 
            aes(x=Y, y= n, color=factor(label), fill=factor(label)))+
    geom_line(aes(linetype=factor(N_IND_DIFF)))+
    theme_bw()+
    ggtitle(paste(com$Var1, com$Var2))+
    scale_colour_manual(values = cols, aesthetics = c("colour", "fill"), guide = 'none')+
    geom_text(data = subset(count_N_IND_DIFF_item, Y == 0),
              aes(label = label, colour = label, x = 0, y = n), hjust = -.1)+
    xlim(c(-1200, 350))
  
  ggplot(count_N_IND_DIFF_item, aes(fill=factor(N_IND_DIFF), y=n, x=Y)) + 
    geom_bar(position="fill", stat="identity") + facet_grid(EVO_TYPE~.)
  
  ggplot(count_N_IND_DIFF_item, aes(fill=factor(N_SP_DIFF), y=n, x=Y)) + 
    geom_bar(position="fill", stat="identity") + facet_grid(EVO_TYPE~.)
  
  #ggsave(p, file=sprintf("%s/Figures/N_Sim/%s_%s_ALL.png", base, com$Var1, com$Var2))
  
  count_N_SP_DIFF_item<-count_N_SP_DIFF %>% filter(NB==com$Var1 & DA==com$Var2)
  count_N_SP_DIFF_item <- count_N_SP_DIFF_item %>% filter(SUITABLE==1)
  count_N_SP_DIFF_item <- count_N_SP_DIFF_item %>% filter(EVO_TYPE!=1)
  count_N_SP_DIFF_item <- count_N_SP_DIFF_item %>% filter(EVO_RATIO!=0.1)
  ggplot(count_N_SP_DIFF_item, aes(fill=factor(N_SP_DIFF), y=n, x=Y)) + 
    geom_bar(position="fill", stat="identity") + facet_grid(EVO_TYPE~.)
  
  count_N_CELL_DIFF_item<-count_N_CELL_DIFF %>% filter(NB==com$Var1 & DA==com$Var2)
  count_N_CELL_DIFF_item <- count_N_CELL_DIFF_item %>% filter(SUITABLE==1)
  count_N_CELL_DIFF_item <- count_N_CELL_DIFF_item %>% filter(EVO_TYPE!=1)
  count_N_CELL_DIFF_item <- count_N_CELL_DIFF_item %>% filter(EVO_RATIO!=0.1)
  ggplot(count_N_CELL_DIFF_item, aes(fill=factor(N_CELL_DIFF), y=n, x=Y)) + 
    geom_bar(position="fill", stat="identity") + facet_grid(EVO_TYPE~.)
  
  mean_sort_N_CELL_item<-mean_sort_N_CELL %>% filter(NB==com$Var1 & DA==com$Var2)
  ggplot(mean_sort_N_CELL_item, 
         aes(x=Y, y= Mean_RANK, color=factor(label), fill=factor(label)))+
    geom_line()+
    theme_bw()+
    ggtitle(paste(com$Var1, com$Var2))+
    #scale_colour_manual(values = cols, aesthetics = c("colour", "fill"), guide = 'none')+
    geom_text(data = subset(mean_sort_N_CELL_item, Y == 0),
              aes(label = label, colour = label, x = 0, y = Mean_RANK), hjust = -.1)+
    xlim(c(-1200, 350))
  
  mean_sort_AVERAGE_N_CELL_item<-mean_sort_AVERAGE_N_CELL %>% filter(NB==com$Var1 & DA==com$Var2)
  ggplot(mean_sort_AVERAGE_N_CELL_item, 
         aes(x=Y, y= Mean_RANK, color=factor(label), fill=factor(label)))+
    geom_line()+
    theme_bw()+
    ggtitle(paste(com$Var1, com$Var2))+
    #scale_colour_manual(values = cols, aesthetics = c("colour", "fill"), guide = 'none')+
    geom_text(data = subset(mean_sort_AVERAGE_N_CELL_item, Y == 0),
              aes(label = label, colour = label, x = 0, y = Mean_RANK), hjust = -.1)+
    xlim(c(-1200, 350))
  
  mean_sort_N_SP_item<-mean_sort_N_SP %>% filter(NB==com$Var1 & DA==com$Var2)
  ggplot(mean_sort_N_SP_item, 
         aes(x=Y, y= Mean_RANK, color=factor(label), fill=factor(label)))+
    geom_line()+
    theme_bw()+
    ggtitle(paste(com$Var1, com$Var2))+
    #scale_colour_manual(values = cols, aesthetics = c("colour", "fill"), guide = 'none')+
    geom_text(data = subset(mean_sort_N_SP_item, Y == 0),
              aes(label = label, colour = label, x = 0, y = Mean_RANK), hjust = -.1)+
    xlim(c(-1200, 350))
}