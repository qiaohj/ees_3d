library(Rmisc)
library(stringr)
library(ggplot2)
library(dplyr)
setwd("~/git/ees_3d/R/smart_species")

result_all<-NULL
evo_type_c<-c(1,2,3,4,5,7,8)
base<-"/home/huijieqiao/git/ees_3d_data/SMART_SPECIES"
for (i in evo_type_c){
  print(i)
  result<-readRDS(sprintf("%s/Tables/individual_ratio_nb_%d.rda", base, i))
  result$Y<-result$Y*-1
  if (is.null(result_all)){
    result_all<-result
  }else{
    result_all<-bind_rows(result_all, result)
  }
}
result_all<-result_all%>%ungroup()
saveRDS(result_all, sprintf("%s/Tables/individual_ratio_nb.rda", base))

