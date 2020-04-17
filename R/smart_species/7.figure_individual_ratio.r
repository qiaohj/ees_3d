library(dplyr)
library(ggplot2)
library(Rmisc)
base<-"~/Downloads"


result<-readRDS(sprintf("%s/Tables/individual_ratio.rda", base))
result$Y<-result$Y*-1
head(result)
mean_df<-result %>%
  dplyr::group_by(Y, SUITABLE, NB, DA, EVO_RATIO, EVO_TYPE) %>%
  dplyr::summarize(Mean_N_IND = mean(N_IND, na.rm=TRUE),
                   Mean_N_SP = mean(N_SP, na.rm=TRUE),
                   Mean_N_CELL = mean(N_CELL, na.rm=TRUE),
                   SD_N_IND = sd(N_IND),
                   SD_N_SP = sd(N_SP),
                   SD_N_CELL = sd(N_CELL),
                   Median_N_IND = quantile(N_IND, na.rm=TRUE, .5),
                   Median_N_SP = quantile(N_SP, na.rm=TRUE, .5),
                   Median_N_CELL = quantile(N_CELL, na.rm=TRUE, .5),
                   CI_N_IND = CI(N_IND, ci=0.95)[2] - CI(N_IND, ci=0.95)[3],
                   CI_N_SP = CI(N_SP, ci=0.95)[2] - CI(N_SP, ci=0.95)[3],
                   CI_N_CELL = CI(N_CELL, ci=0.95)[2] - CI(N_CELL, ci=0.95)[3],
                   N=length(unique(GLOBAL_ID))
  )


cols<-c("MODERATE GOOD 0.01 2"="deepskyblue",
        "MODERATE GOOD 0.1 2"="deepskyblue3",
        "MODERATE GOOD 0.01 3"="khaki1",
        "MODERATE GOOD 0.1 3"="khaki3",
        "MODERATE GOOD 0.01 4"="hotpink",
        "MODERATE GOOD 0.1 4"="hotpink3",
        "MODERATE GOOD 1 1"="black")

mean_df[is.na(mean_df)]<-0
mean_df$label<-paste(mean_df$NB, mean_df$DA, mean_df$EVO_RATIO, mean_df$EVO_TYPE)

mean_df_item<-mean_df %>% filter(NB=="MODERATE" & DA=="GOOD")

ggplot(mean_df_item %>% filter(SUITABLE==0))+geom_line(aes(x=Y, y= N, 
                                                           color=factor(label)))+
  theme_bw()+
  scale_colour_manual(values = cols,aesthetics = c("colour", "fill"))


ggplot(mean_df_item)+geom_line(aes(x=Y, y= Median_N_IND, 
                                   linetype=factor(SUITABLE), color=factor(label)))+
  scale_y_log10()+
  theme_bw()

ggplot(mean_df_item %>% filter(SUITABLE==1 & Y>=-1190), 
       aes(x=Y, y= Mean_N_CELL, color=factor(label), fill=factor(label)))+
  geom_line()+
  geom_ribbon(aes(ymin=Mean_N_CELL-CI_N_CELL, ymax=Mean_N_CELL+CI_N_CELL))+
  theme_bw()+
  scale_colour_manual(values = cols,aesthetics = c("colour", "fill"))

ggplot(mean_df_item %>% filter(SUITABLE==1 & Y>=-1190))+geom_line(aes(x=Y, y= Mean_N_SP, 
                                                                      color=factor(label)))+
  scale_y_log10()+
  theme_bw()+
  scale_colour_manual(values = cols,aesthetics = c("colour", "fill"))
