
library(stringr)
library(ggplot2)
library(Rmisc)
library(ggpubr)
library(dplyr)

setwd("~/git/ees_3d/R/smart_species")
base<-"/home/huijieqiao/git/ees_3d_data/SMART_SPECIES"
function(x){
  x[which(x==1)]<-"Lazy"
  x[which(x==2)]<-"Darwin"
  x[which(x==3)]<-"Lamarck"
  x[which(x==4)]<-"AI"
  x[which(x==5)]<-"Lamarck II"
  x[which(x==6)]<-"Darwin xII"
  x[which(x==7)]<-"Darwin II"
  x[which(x==8)]<-"Combined"
  x
}

fix_df<-function(p_df){
  #p_df<-p_df %>% filter((EVO_TYPE!=6))
  p_df[which(p_df$EVO_RATIO==0.01), "EVO_RATIO"]<-0.005
  p_df[which(p_df$EVO_RATIO==0.1), "EVO_RATIO"]<-0.05
  p_df_1<-p_df %>%filter(EVO_RATIO==1)
  p_df_1$EVO_RATIO<-0.05
  p_df[which(p_df$EVO_RATIO==1), "EVO_RATIO"]<-0.005
  p_df<-bind_rows(p_df, p_df_1)
  #p_df$EVO_RATIO<-1
  p_df$EVO_TYPE<-fix_type(p_df$EVO_TYPE)
  p_df$WARP_LABEL<-paste(p_df$NB, p_df$DA, p_df$EVO_RATIO)
  p_df
}

mean_sort_AVERAGE_N_CELL<-readRDS("../../../ees_3d_data/SMART_SPECIES/Tables/9.null_model_comparison_analysis/mean_sort_AVERAGE_N_CELL_Fixed.rda")
mean_sort_N_CELL<-readRDS("../../../ees_3d_data/SMART_SPECIES/Tables/9.null_model_comparison_analysis/mean_sort_N_CELL_Fixed.rda")
mean_sort_N_SP<-readRDS("../../../ees_3d_data/SMART_SPECIES/Tables/9.null_model_comparison_analysis/mean_sort_N_SP_Fixed.rda")



mean_df_ratio_df<-readRDS("../../../ees_3d_data/SMART_SPECIES/Tables/9.null_model_comparison_analysis/mean_df_ratio_df.rda")
mean_df_ratio_df<-fix_df(mean_df_ratio_df)




count_N_IND_DIFF<-readRDS("../../../ees_3d_data/SMART_SPECIES/Tables/9.null_model_comparison_analysis/count_N_IND_DIFF_Fixed.rda")
count_N_SP_DIFF<-readRDS("../../../ees_3d_data/SMART_SPECIES/Tables/9.null_model_comparison_analysis/count_N_SP_DIFF_Fixed.rda")
count_N_CELL_DIFF<-readRDS("../../../ees_3d_data/SMART_SPECIES/Tables/9.null_model_comparison_analysis/count_N_CELL_DIFF_Fixed.rda")


cols<-c("red",
        "orange",
        "purple",
        "turquoise",
        "blue",
        "green",
        "black")

names(cols)<-unique(N_Sim$EVO_TYPE)
x_year_label<-"Year (*100)"
ribbon_alpha<-0.1

#--------------------------------------------#
#Figure: N running simulations per time step #
#--------------------------------------------#

N_Sim<-readRDS("../../../ees_3d_data/SMART_SPECIES/Tables/9.null_model_comparison_analysis/N_Sim.rda")
N_Sim<-fix_df(N_Sim)

p<-ggplot(N_Sim, 
          aes(x=Y, y= N, color=EVO_TYPE, fill=EVO_TYPE))+
  geom_line()+
  theme_bw()+
  xlab(x_year_label)+
  ylab("N running simulations")+
  ggtitle("N running simulations per time step")+
  scale_colour_manual(values = cols, aesthetics = c("colour", "fill"), guide = 'none')+
  geom_text(data = subset(N_Sim, Y == 0), 
            aes(label = EVO_TYPE, colour = EVO_TYPE, x = 0, y = N), hjust = -.1)+
  xlim(c(-1200, 350))+
  facet_wrap( ~ WARP_LABEL, ncol=2, scales = 'free')

ggsave(p, file=sprintf("%s/Figures/N_Sim_ALL.png", base), width = 10, height=12)

p<-ggplot(N_Sim %>% filter((Y>=-1000)&(EVO_TYPE!='Lamarck')), 
          aes(x=Y, y= N, color=EVO_TYPE, fill=EVO_TYPE))+
  geom_line()+
  theme_bw()+
  xlab(x_year_label)+
  ylab("N running simulations")+
  ggtitle("N running simulations per time step")+
  scale_colour_manual(values = cols,aesthetics = c("colour", "fill"), guide = 'none')+
  geom_text(data = N_Sim %>% filter((Y==0)&(EVO_TYPE!='Lamarck')), 
            aes(label = EVO_TYPE, colour = EVO_TYPE, x = 0, y = N), hjust = -.1)+
  xlim(c(-1000, 400))+
  facet_wrap( ~ WARP_LABEL, ncol=2, scales = 'free')
ggsave(p, file=sprintf("%s/Figures/N_Sim_PART.png", base), width = 10, height=12)

#--------------------------------------------------#
#Figure: Mean number of species of all simulations #
#--------------------------------------------------#
mean_df<-readRDS("../../../ees_3d_data/SMART_SPECIES/Tables/9.null_model_comparison_analysis/mean_df.rda")
#function fix_df() is in9.null_model_comparision_analysis.r
mean_df<-fix_df(mean_df)

p<-ggplot(mean_df %>% filter(SUITABLE==1), 
          aes(x=Y, y=Mean_N_SP))+
  geom_line(aes(color=EVO_TYPE))+
  geom_ribbon(aes(ymin=Mean_N_SP-CI_N_SP, ymax=Mean_N_SP+CI_N_SP, fill=EVO_TYPE), 
              color=NA, alpha=ribbon_alpha)+
  theme_bw()+
  xlab(x_year_label)+
  ylab("Mean N of species")+
  ggtitle("Mean number of species of all simulations")+
  scale_colour_manual(values = cols, aesthetics = c("colour", "fill"), guide = 'none')+
  geom_text(data = mean_df %>% filter((Y==0)&(SUITABLE==1)), 
            aes(label = EVO_TYPE, colour = EVO_TYPE, x = 0, y = Mean_N_SP), hjust = -.1)+
  xlim(c(-1200, 350))+
  facet_wrap( ~ WARP_LABEL, ncol=2, scales = 'free')
ggsave(p, file=sprintf("%s/Figures/N_Species_ALL.png", base), width = 10, height=12)

p<-ggplot(mean_df %>% filter(SUITABLE==1), 
          aes(x=Y, y=Mean_N_CELL))+
  geom_line(aes(color=EVO_TYPE))+
  geom_ribbon(aes(ymin=Mean_N_CELL-CI_N_CELL, ymax=Mean_N_CELL+CI_N_CELL, fill=EVO_TYPE), 
              color=NA, alpha=0.1)+
  theme_bw()+
  xlab(x_year_label)+
  ylab("Mean number of suitable pixels")+
  ggtitle("Mean number of suitable pixels of all simulations")+
  scale_colour_manual(values = cols, aesthetics = c("colour", "fill"), guide = 'none')+
  geom_text(data = mean_df %>% filter((Y==0)&(SUITABLE==1)), 
            aes(label = EVO_TYPE, colour = EVO_TYPE, x = 0, y = Mean_N_CELL), hjust = -.1)+
  xlim(c(-1200, 350))+
  facet_wrap( ~ WARP_LABEL, ncol=2, scales = 'free')
ggsave(p, file=sprintf("%s/Figures/N_Cells_ALL_SUITABLE.png", base), width = 10, height=12)









comb<-expand.grid(unique(N_Sim$NB), unique(N_Sim$DA), 
                  stringsAsFactors=F)
i=1

for (i in c(1:nrow(comb))){
  com<-comb[i,]
  mean_df_item<-mean_df %>% filter(NB==com$Var1 & DA==com$Var2)
  mean_df_ratio_df_item<-mean_df_ratio_df %>% filter(NB==com$Var1 & DA==com$Var2)
  N_Sim_item<-N_Sim %>% filter(NB==com$Var1 & DA==com$Var2)
  max_N<-N_Sim_item[which(N_Sim_item$Y==-1199), ]
  N_Sim_item<-left_join(N_Sim_item, max_N[, c("NB", "DA", "EVO_RATIO", "EVO_TYPE", "label", "N")], by=c("NB", "DA", "EVO_RATIO", "EVO_TYPE", "label"))
  N_Sim_item$N_AVE<-N_Sim_item$N.x/N_Sim_item$N.y
  unique(N_Sim_item$label)
  
  N_Sim_item %>% filter(Y==0)
  
  
  
  p<-ggplot(mean_df_item %>% filter(SUITABLE==0), 
            aes(x=Y, y=Mean_N_CELL))+
    geom_line(aes(color=factor(label)))+
    #geom_ribbon(aes(ymin=Mean_N_CELL-CI_N_CELL, ymax=Mean_N_CELL+CI_N_CELL, fill=factor(label)), color=NA, alpha=ribbon_alpha)+
    theme_bw()+
    xlab(x_year_label)+
    ylab("Mean number of unsuitable pixels of all simulations")+
    ggtitle(paste(com$Var1, com$Var2))+
    scale_colour_manual(values = cols,aesthetics = c("colour", "fill"), guide = 'none')+
    geom_text(data = mean_df_item %>% filter((Y==0)&(SUITABLE==0)), aes(label = label, colour = label, x = 0, y = Mean_N_CELL), hjust = -.1)+
    xlim(c(-1200, 350))
  ggsave(p, file=sprintf("%s/Figures/N_Cells/%s_%s_ALL_UNSUITABLE.png", base, com$Var1, com$Var2))
  
  
  p<-ggplot(mean_df_item %>% filter((SUITABLE==1)&(Y<=-550)), 
            aes(x=Y, y=Mean_N_CELL))+
    geom_line(aes(color=factor(label)))+
    #geom_ribbon(aes(ymin=Mean_N_CELL-CI_N_CELL, ymax=Mean_N_CELL+CI_N_CELL, fill=factor(label)), color=NA, alpha=ribbon_alpha)+
    theme_bw()+
    xlab(x_year_label)+
    ylab("Mean number of pixels of all simulations")+
    ggtitle(paste(com$Var1, com$Var2))+
    scale_colour_manual(values = cols,aesthetics = c("colour", "fill"), guide = 'none')+
    geom_text(data = mean_df_item %>% filter((Y==-550)&(SUITABLE==1)), aes(label = label, colour = label, x = -550, y = Mean_N_CELL), hjust = -.1)+
    xlim(c(-1200, -400))
  ggsave(p, file=sprintf("%s/Figures/N_Cells/%s_%s_1100.png", base, com$Var1, com$Var2))
  
  p<-ggplot(mean_df_item %>% filter(SUITABLE==1), 
            aes(x=Y, y=Mean_AVERAGE_N_CELL))+
    geom_line(aes(color=factor(label)))+
    #geom_ribbon(aes(ymin=Mean_AVERAGE_N_CELL-CI_AVERAGE_N_CELL, ymax=Mean_AVERAGE_N_CELL+CI_AVERAGE_N_CELL, fill=factor(label)), color=NA, alpha=ribbon_alpha)+
    theme_bw()+
    xlab(x_year_label)+
    ylab("Mean number of suitable pixels per species")+
    ggtitle(paste(com$Var1, com$Var2))+
    scale_colour_manual(values = cols,aesthetics = c("colour", "fill"), guide = 'none')+
    geom_text(data = mean_df_item %>% filter((Y==0)&(SUITABLE==1)), aes(label = label, colour = label, x = 0, y = Mean_AVERAGE_N_CELL), hjust = -.1)+
    xlim(c(-1200, 350))
  
  
  ggsave(p, file=sprintf("%s/Figures/N_CELLs/%s_%s_AVERAGE_SUITABLE.png", base, com$Var1, com$Var2))
  
  p<-ggplot(mean_df_item %>% filter(SUITABLE==0), 
            aes(x=Y, y=Mean_AVERAGE_N_CELL))+
    geom_line(aes(color=factor(label)))+
    #geom_ribbon(aes(ymin=Mean_AVERAGE_N_CELL-CI_AVERAGE_N_CELL, ymax=Mean_AVERAGE_N_CELL+CI_AVERAGE_N_CELL, fill=factor(label)), color=NA, alpha=ribbon_alpha)+
    theme_bw()+
    xlab(x_year_label)+
    ylab("Mean number of unsuitable pixels per species")+
    ggtitle(paste(com$Var1, com$Var2))+
    scale_colour_manual(values = cols,aesthetics = c("colour", "fill"), guide = 'none')+
    geom_text(data = mean_df_item %>% filter((Y==0)&(SUITABLE==0)), aes(label = label, colour = label, x = 0, y = Mean_AVERAGE_N_CELL), hjust = -.1)+
    xlim(c(-1200, 350))
  
  
  ggsave(p, file=sprintf("%s/Figures/N_CELLs/%s_%s_AVERAGE_UNSUITABLE.png", base, com$Var1, com$Var2))
  
  p<-ggplot(mean_df_ratio_df_item %>% filter(Y>=-1198), aes(x=Y, y=Mean_ratio))+
    geom_line(aes(color=factor(label)))+
    geom_ribbon(aes(ymin=Mean_ratio-CI_ratio, ymax=Mean_ratio+CI_ratio, fill=factor(label)), color=NA, alpha=ribbon_alpha)+
    theme_bw()+
    xlab(x_year_label)+
    ylab("Mean number of unsuitable individuals/all individuals")+
    ggtitle(paste(com$Var1, com$Var2))+
    scale_colour_manual(values = cols,aesthetics = c("colour", "fill"), guide = 'none')+
    geom_text(data = mean_df_ratio_df_item %>% filter((Y==0)), aes(label = label, colour = label, x = 0, y = Mean_ratio), hjust = -.1)+
    xlim(c(-1200, 350))
  
  p <- p + ylim(0, 1)
  
  ggsave(p, file=sprintf("%s/Figures/Trial_Error/%s_%s_ALL.png", base, com$Var1, com$Var2))
  
  p<-ggplot(mean_df_ratio_df_item %>% filter((Y<=-1100)&(Y>=-1198)&(EVO_TYPE!=3)), aes(x=Y, y=Mean_ratio))+
    geom_line(aes(color=factor(label)))+
    #geom_ribbon(aes(ymin=Mean_ratio-CI_ratio, ymax=Mean_ratio+CI_ratio, fill=factor(label)), color=NA, alpha=ribbon_alpha)+
    theme_bw()+
    xlab(x_year_label)+
    ylab("Mean number of unsuitable individuals/all individuals")+
    ggtitle(paste(com$Var1, com$Var2))+
    scale_colour_manual(values = cols,aesthetics = c("colour", "fill"), guide = 'none')+
    geom_text(data = mean_df_ratio_df_item %>% filter((Y==-1100)&(EVO_TYPE!=3)), aes(label = label, colour = label, x = -1100, y = Mean_ratio), hjust = -.1)+
    xlim(c(-1198, -1050))
  ggsave(p, file=sprintf("%s/Figures/Trial_Error/%s_%s_1100.png", base, com$Var1, com$Var2))
  
  
  count_N_IND_DIFF_item<-count_N_IND_DIFF %>% filter(NB==com$Var1 & DA==com$Var2)
  count_N_IND_DIFF_item <- count_N_IND_DIFF_item %>% filter(SUITABLE==1)
  count_N_IND_DIFF_item <- count_N_IND_DIFF_item %>% filter(EVO_TYPE!="Lazy")
  
  p1<-ggplot(count_N_IND_DIFF_item %>%filter(N_IND_DIFF==-1), 
            aes(x=Y, y= n, color=factor(label), fill=factor(label)))+
    geom_line()+
    theme_bw()+
    ggtitle(paste(com$Var1, com$Var2))+
    scale_colour_manual(values = cols, aesthetics = c("colour", "fill"), guide = 'none')+
    geom_text(data = count_N_IND_DIFF_item %>%filter((Y==0)&(N_IND_DIFF==-1)),
              aes(label = label, colour = label, x = 0, y = n), hjust = -.1)+
    xlim(c(-1200, 350))
  p2<-ggplot(count_N_IND_DIFF_item %>%filter(N_IND_DIFF==1), 
             aes(x=Y, y= n, color=factor(label), fill=factor(label)))+
    geom_line()+
    theme_bw()+
    ggtitle(paste(com$Var1, com$Var2))+
    scale_colour_manual(values = cols, aesthetics = c("colour", "fill"), guide = 'none')+
    geom_text(data = count_N_IND_DIFF_item %>%filter((Y==0)&(N_IND_DIFF==1)),
              aes(label = label, colour = label, x = 0, y = n), hjust = -.1)+
    xlim(c(-1200, 350))
  p3<-ggplot(count_N_IND_DIFF_item %>%filter(N_IND_DIFF==0), 
             aes(x=Y, y= n, color=factor(label), fill=factor(label)))+
    geom_line()+
    theme_bw()+
    ggtitle(paste(com$Var1, com$Var2))+
    scale_colour_manual(values = cols, aesthetics = c("colour", "fill"), guide = 'none')+
    geom_text(data = count_N_IND_DIFF_item %>%filter((Y==0)&(N_IND_DIFF==0)),
              aes(label = label, colour = label, x = 0, y = n), hjust = -.1)+
    xlim(c(-1200, 350))
  
  ggarrange(p1, p2, ncol = 1, nrow = 2)
  
  
  ggplot(count_N_IND_DIFF_item, aes(fill=factor(N_IND_DIFF), y=n, x=Y)) + 
    geom_bar(position="fill", stat="identity") + facet_grid(EVO_TYPE~.)
  
  ggplot(count_N_IND_DIFF_item, aes(fill=factor(N_SP_DIFF), y=n, x=Y)) + 
    geom_bar(position="fill", stat="identity") + facet_grid(EVO_TYPE~.)
  
  #ggsave(p, file=sprintf("%s/Figures/N_Sim/%s_%s_ALL.png", base, com$Var1, com$Var2))
  
  count_N_SP_DIFF_item<-count_N_SP_DIFF %>% filter(NB==com$Var1 & DA==com$Var2)
  count_N_SP_DIFF_item <- count_N_SP_DIFF_item %>% filter(SUITABLE==1)
  count_N_SP_DIFF_item <- count_N_SP_DIFF_item %>% filter(EVO_TYPE!=1)
  count_N_SP_DIFF_item <- count_N_SP_DIFF_item %>% filter(EVO_RATIO!=0.1)
  p1<-ggplot(count_N_SP_DIFF_item %>%filter(N_SP_DIFF==-1), 
             aes(x=Y, y= n, color=factor(label), fill=factor(label)))+
    geom_line()+
    theme_bw()+
    ggtitle(paste(com$Var1, com$Var2))+
    scale_colour_manual(values = cols, aesthetics = c("colour", "fill"), guide = 'none')+
    geom_text(data = count_N_SP_DIFF_item %>%filter((Y==0)&(N_SP_DIFF==-1)),
              aes(label = label, colour = label, x = 0, y = n), hjust = -.1)+
    xlim(c(-1200, 350))
  p2<-ggplot(count_N_SP_DIFF_item %>%filter(N_SP_DIFF==1), 
             aes(x=Y, y= n, color=factor(label), fill=factor(label)))+
    geom_line()+
    theme_bw()+
    ggtitle(paste(com$Var1, com$Var2))+
    scale_colour_manual(values = cols, aesthetics = c("colour", "fill"), guide = 'none')+
    geom_text(data = count_N_SP_DIFF_item %>%filter((Y==0)&(N_SP_DIFF==1)),
              aes(label = label, colour = label, x = 0, y = n), hjust = -.1)+
    xlim(c(-1200, 350))
  p3<-ggplot(count_N_SP_DIFF_item %>%filter(N_SP_DIFF==0), 
             aes(x=Y, y= n, color=factor(label), fill=factor(label)))+
    geom_line()+
    theme_bw()+
    ggtitle(paste(com$Var1, com$Var2))+
    scale_colour_manual(values = cols, aesthetics = c("colour", "fill"), guide = 'none')+
    geom_text(data = count_N_SP_DIFF_item %>%filter((Y==0)&(N_SP_DIFF==0)),
              aes(label = label, colour = label, x = 0, y = n), hjust = -.1)+
    xlim(c(-1200, 350))
  
  ggarrange(p1, p2, ncol = 1, nrow = 2)
  
  ggplot(count_N_SP_DIFF_item, aes(fill=factor(N_SP_DIFF), y=n, x=Y)) + 
    geom_bar(position="fill", stat="identity") + facet_grid(EVO_TYPE~.)
  
  count_N_CELL_DIFF_item<-count_N_CELL_DIFF %>% filter(NB==com$Var1 & DA==com$Var2)
  count_N_CELL_DIFF_item <- count_N_CELL_DIFF_item %>% filter(SUITABLE==1)
  count_N_CELL_DIFF_item <- count_N_CELL_DIFF_item %>% filter(EVO_TYPE!=1)
  count_N_CELL_DIFF_item <- count_N_CELL_DIFF_item %>% filter(EVO_RATIO!=0.1)
  p1<-ggplot(count_N_CELL_DIFF_item %>%filter(N_CELL_DIFF==-1), 
             aes(x=Y, y= n, color=factor(label), fill=factor(label)))+
    geom_line()+
    theme_bw()+
    ggtitle(paste(com$Var1, com$Var2))+
    scale_colour_manual(values = cols, aesthetics = c("colour", "fill"), guide = 'none')+
    geom_text(data = count_N_CELL_DIFF_item %>%filter((Y==0)&(N_CELL_DIFF==-1)),
              aes(label = label, colour = label, x = 0, y = n), hjust = -.1)+
    xlim(c(-1200, 350))
  p2<-ggplot(count_N_CELL_DIFF_item %>%filter(N_CELL_DIFF==1), 
             aes(x=Y, y= n, color=factor(label), fill=factor(label)))+
    geom_line()+
    theme_bw()+
    ggtitle(paste(com$Var1, com$Var2))+
    scale_colour_manual(values = cols, aesthetics = c("colour", "fill"), guide = 'none')+
    geom_text(data = count_N_CELL_DIFF_item %>%filter((Y==0)&(N_CELL_DIFF==1)),
              aes(label = label, colour = label, x = 0, y = n), hjust = -.1)+
    xlim(c(-1200, 350))
  p3<-ggplot(count_N_CELL_DIFF_item %>%filter(N_CELL_DIFF==0), 
             aes(x=Y, y= n, color=factor(label), fill=factor(label)))+
    geom_line()+
    theme_bw()+
    ggtitle(paste(com$Var1, com$Var2))+
    scale_colour_manual(values = cols, aesthetics = c("colour", "fill"), guide = 'none')+
    geom_text(data = count_N_CELL_DIFF_item %>%filter((Y==0)&(N_CELL_DIFF==0)),
              aes(label = label, colour = label, x = 0, y = n), hjust = -.1)+
    xlim(c(-1200, 350))
  
  ggarrange(p1, p2, ncol = 1, nrow = 2)
  
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