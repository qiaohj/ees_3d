library(dplyr)
library(ggplot2)
base<-"C:/Users/Huijie Qiao/Downloads"

result<-readRDS(sprintf("%s/Tables/individual_ratio.rda", base))
result$Y<-result$Y*-1
items<-strsplit(result$LABLE, "_")
names(items)<-c(1:length(items))
items_df<-bind_rows(items)

mean_df<-result %>%
  group_by(Y, SUITABLE, NB, DA, EVO_RATIO) %>%
  summarize(Mean_N_IND = mean(N_IND, na.rm=TRUE),
            Mean_N_SP = mean(N_SP, na.rm=TRUE),
            Mean_N_CELL = mean(N_CELL, na.rm=TRUE),
            SD_N_IND = sd(N_IND),
            SD_N_SP = sd(N_SP),
            SD_N_CELL = sd(N_CELL)
  )

mean_df$label<-paste(mean_df$NB, mean_df$DA, mean_df$EVO_RATIO)

ggplot(mean_df)+geom_line(aes(x=Y, y= Mean_N_IND, 
                              linetype=factor(SUITABLE), color=factor(label)))+
  scale_y_log10()+
  theme_bw()
