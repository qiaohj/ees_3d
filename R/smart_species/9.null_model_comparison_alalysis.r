
library(dplyr)
library(stringr)
library(ggplot2)
library(Rmisc)
setwd("~/git/ees_3d/R/smart_species")
base<-"/home/huijieqiao/git/ees_3d_data/SMART_SPECIES"
result<-readRDS(sprintf("%s/Tables/individual_ratio.rda", base))
result$Y<-result$Y*-1
head(result)
result$AVERAGE_N_CELL<-result$N_CELL/result$N_SP

NULL_DF<-result %>% filter(EVO_TYPE==1)
colnames(NULL_DF)[c(3, 4, 5, 6, 9, 11, 12)]<-
  paste("NULL", colnames(NULL_DF)[c(3, 4, 5, 6, 9, 11, 12)], sep="_")


FULL_DF<-expand.grid(GLOBAL_ID=unique(NULL_DF$GLOBAL_ID),
                     DA=unique(NULL_DF$DA),
                     NB=unique(NULL_DF$NB),
                     Y=unique(NULL_DF$Y),
                     SUITABLE=unique(NULL_DF$SUITABLE),
                     stringsAsFactors = F
                     )
FULL_DF<-left_join(FULL_DF, NULL_DF, by=c("Y", "SUITABLE", "NB", "DA", "GLOBAL_ID"))
FULL_DF$NULL_EVO_TYPE<-1
FULL_DF$NULL_EVO_RATIO<-1
FULL_DF[is.na(FULL_DF)]<-0
FULL_DF<-FULL_DF[, c("GLOBAL_ID", "DA", "NB", "Y", "SUITABLE", "NULL_N_IND", 
                     "NULL_N_SP", "NULL_N_CELL", "NULL_AVERAGE_N_CELL")]

FULL_DF_2<-NULL

for (EVO_TYPE in c(1:7)){
  print(EVO_TYPE)
  if (EVO_TYPE==1){
    ITEM<-FULL_DF
    ITEM$EVO_TYPE<-EVO_TYPE
    ITEM$EVO_RATIO<-1
  }else{
    FULL_DF_0.1<-FULL_DF
    FULL_DF_0.1$EVO_TYPE<-EVO_TYPE
    FULL_DF_0.1$EVO_RATIO<-0.1
    
    FULL_DF_0.01<-FULL_DF
    FULL_DF_0.01$EVO_TYPE<-EVO_TYPE
    FULL_DF_0.01$EVO_RATIO<-0.01
    ITEM<-bind_rows(FULL_DF_0.1, FULL_DF_0.01)
  }
  if (is.null(FULL_DF_2)){
    FULL_DF_2<-ITEM
  }else{
    FULL_DF_2<-bind_rows(FULL_DF_2, ITEM)
  }
}

ALL_DF<-left_join(FULL_DF_2, result, by=c("Y", "SUITABLE", "NB", "DA", "GLOBAL_ID", "EVO_TYPE", "EVO_RATIO"))
dim(ALL_DF)
head(ALL_DF)
tail(ALL_DF)
ALL_DF$LABLE<-paste(ALL_DF$GLOBAL_ID, ALL_DF$NB, ALL_DF$DA, ALL_DF$EVO_RATIO, ALL_DF$EVO_TYPE, sep="_")
ALL_DF<-ALL_DF%>%filter((EVO_TYPE!=6)&(EVO_RATIO!=0.1))

ALL_DF[is.na(ALL_DF)]<-0

ALL_DF$N_IND_DIFF<-0
ALL_DF[which(ALL_DF$N_IND>ALL_DF$NULL_N_IND), "N_IND_DIFF"]<-1
ALL_DF[which(ALL_DF$N_IND<ALL_DF$NULL_N_IND), "N_IND_DIFF"]<--1

ALL_DF$N_SP_DIFF<-0
ALL_DF[which(ALL_DF$N_SP>ALL_DF$NULL_N_SP), "N_SP_DIFF"]<-1
ALL_DF[which(ALL_DF$N_SP<ALL_DF$NULL_N_SP), "N_SP_DIFF"]<--1

ALL_DF$N_CELL_DIFF<-0
ALL_DF[which(ALL_DF$N_CELL>ALL_DF$NULL_N_CELL), "N_CELL_DIFF"]<-1
ALL_DF[which(ALL_DF$N_CELL<ALL_DF$NULL_N_CELL), "N_CELL_DIFF"]<--1

saveRDS(ALL_DF, "../../../ees_3d_data/SMART_SPECIES/Tables/9.null_model_comparison_analysis/ALL_DF.rda")
#table(ALL_DF$N_IND_DIFF)
#ALL_DF<-readRDS("../../../ees_3d_data/SMART_SPECIES/Tables/9.null_model_comparison_analysis/ALL_DF.rda")
count_N_IND_DIFF<-ALL_DF %>% group_by(Y, SUITABLE, NB, DA, N_IND_DIFF, EVO_TYPE, EVO_RATIO)%>% count()
count_N_IND_DIFF$label<-paste(count_N_IND_DIFF$NB, count_N_IND_DIFF$DA, count_N_IND_DIFF$EVO_RATIO, count_N_IND_DIFF$EVO_TYPE)

saveRDS(count_N_IND_DIFF, "../../../ees_3d_data/SMART_SPECIES/Tables/9.null_model_comparison_analysis/count_N_IND_DIFF.rda")

count_N_SP_DIFF<-ALL_DF %>% group_by(Y, SUITABLE, NB, DA, N_SP_DIFF, EVO_TYPE, EVO_RATIO)%>% count()
count_N_SP_DIFF$label<-paste(count_N_SP_DIFF$NB, count_N_SP_DIFF$DA, count_N_SP_DIFF$EVO_RATIO, count_N_SP_DIFF$EVO_TYPE)
saveRDS(count_N_SP_DIFF, "../../../ees_3d_data/SMART_SPECIES/Tables/9.null_model_comparison_analysis/count_N_SP_DIFF.rda")

count_N_CELL_DIFF<-ALL_DF %>% group_by(Y, SUITABLE, NB, DA, N_CELL_DIFF, EVO_TYPE, EVO_RATIO)%>% count()
count_N_CELL_DIFF$label<-paste(count_N_CELL_DIFF$NB, count_N_CELL_DIFF$DA, count_N_CELL_DIFF$EVO_RATIO, count_N_CELL_DIFF$EVO_TYPE)
saveRDS(count_N_CELL_DIFF, "../../../ees_3d_data/SMART_SPECIES/Tables/9.null_model_comparison_analysis/count_N_CELL_DIFF.rda")

sort_N_CELL<-ALL_DF%>% dplyr::filter((SUITABLE==1)&(EVO_RATIO!=0.1)) %>%
  dplyr::group_by(Y, GLOBAL_ID, DA, NB) %>% 
  dplyr::mutate(RANK = rank(N_CELL, ties.method = "average"))
saveRDS(sort_N_CELL, "../../../ees_3d_data/SMART_SPECIES/Tables/9.null_model_comparison_analysis/sort_N_CELL.rda")


range(sort_N_CELL$RANK)

mean_sort_N_CELL<-sort_N_CELL %>%
  dplyr::group_by(Y, NB, DA, EVO_TYPE) %>%
  dplyr::summarize(Mean_RANK = mean(RANK, na.rm=TRUE),
                   SD_RANK = sd(RANK),
                   Median_RANK = quantile(RANK, na.rm=TRUE, .5),
                   CI_RANK = CI(RANK, ci=0.95)[2] - CI(RANK, ci=0.95)[3]
  )
mean_sort_N_CELL[is.na(mean_sort_N_CELL)]<-0
mean_sort_N_CELL$label<-paste(mean_sort_N_CELL$NB, mean_sort_N_CELL$DA, mean_sort_N_CELL$EVO_TYPE)
saveRDS(mean_sort_N_CELL, "../../../ees_3d_data/SMART_SPECIES/Tables/9.null_model_comparison_analysis/mean_sort_N_CELL.rda")

sort_AVERAGE_N_CELL<-ALL_DF%>%  
  dplyr::filter((SUITABLE==1)&(EVO_RATIO!=0.1)) %>%
  dplyr::group_by(Y, GLOBAL_ID, DA, NB)%>%
  dplyr::mutate(RANK = rank(AVERAGE_N_CELL, ties.method = "average"))
range(sort_AVERAGE_N_CELL$RANK)
saveRDS(sort_AVERAGE_N_CELL, "../../../ees_3d_data/SMART_SPECIES/Tables/9.null_model_comparison_analysis/sort_AVERAGE_N_CELL.rda")


mean_sort_AVERAGE_N_CELL<-sort_AVERAGE_N_CELL %>%
  dplyr::group_by(Y, NB, DA, EVO_TYPE) %>%
  dplyr::summarize(Mean_RANK = mean(RANK, na.rm=TRUE),
                   SD_RANK = sd(RANK),
                   Median_RANK = quantile(RANK, na.rm=TRUE, .5),
                   CI_RANK = CI(RANK, ci=0.95)[2] - CI(RANK, ci=0.95)[3]
  )
mean_sort_AVERAGE_N_CELL[is.na(mean_sort_AVERAGE_N_CELL)]<-0
mean_sort_AVERAGE_N_CELL$label<-paste(mean_sort_AVERAGE_N_CELL$NB, mean_sort_AVERAGE_N_CELL$DA, mean_sort_AVERAGE_N_CELL$EVO_TYPE)
saveRDS(mean_sort_AVERAGE_N_CELL, "../../../ees_3d_data/SMART_SPECIES/Tables/9.null_model_comparison_analysis/mean_sort_AVERAGE_N_CELL.rda")

sort_N_SP<-ALL_DF%>%  
  dplyr::filter((SUITABLE==1)&(EVO_RATIO!=0.1)) %>%
  dplyr::group_by(Y, GLOBAL_ID, DA, NB)%>%
  dplyr::mutate(RANK = rank(N_SP, ties.method = "average"))
range(sort_N_SP$RANK)
saveRDS(sort_N_SP, "../../../ees_3d_data/SMART_SPECIES/Tables/9.null_model_comparison_analysis/sort_N_SP.rda")


mean_sort_N_SP<-sort_N_SP %>%
  dplyr::group_by(Y, NB, DA, EVO_TYPE) %>%
  dplyr::summarize(Mean_RANK = mean(RANK, na.rm=TRUE),
                   SD_RANK = sd(RANK),
                   Median_RANK = quantile(RANK, na.rm=TRUE, .5),
                   CI_RANK = CI(RANK, ci=0.95)[2] - CI(RANK, ci=0.95)[3]
  )
mean_sort_N_SP[is.na(mean_sort_N_SP)]<-0
mean_sort_N_SP$label<-paste(mean_sort_N_SP$NB, mean_sort_N_SP$DA, mean_sort_N_SP$EVO_TYPE)
saveRDS(mean_sort_N_SP, "../../../ees_3d_data/SMART_SPECIES/Tables/9.null_model_comparison_analysis/mean_sort_N_SP.rda")


test<-sort_N_SP%>%filter((Y==-1199)&(GLOBAL_ID==24683)&(DA=="POOR")&(NB=="NARROW"))
rank(test$N_SP, ties.method="average")


