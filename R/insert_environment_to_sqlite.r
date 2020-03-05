library(RSQLite)
library(DBI)
library(dplyr)
library(stringr)

folders<-c("Debiased_Maximum_Monthly_Precipitation",
           "Debiased_Maximum_Monthly_Temperature",
           "Debiased_Mean_Annual_Precipitation",
           "Debiased_Mean_Annual_Temperature",
           "Debiased_Minimum_Monthly_Temperature")

base<-"/home/huijieqiao/git/ees_3d_data/ISEA3H8"
args = commandArgs(trailingOnly=TRUE)
folder<-folders[1]
folder<-folders[as.numeric(args[1])]
# Create an ephemeral in-memory RSQLite database
mydb <- dbConnect(RSQLite::SQLite(), sprintf("%s/SQLITE/env_Hadley3D.sqlite", base))
#dbReadTable(mydb, "mask")
mask<-read.csv(sprintf("%s/CSV/mask.csv", base), sep=" ", head=T)
mask$v<-1
dbWriteTable(mydb, "mask", mask, overwrite=T)

y=0

df_all<-NULL
for (y in c(0:1200)){
  print(y)
  df<-read.csv(sprintf("%s/CSV/%s/%s.csv", base, folder, str_pad(y, 4, pad="0")), sep=" ", head=T)
  colnames(df)<-c("global_id", "v")
  df$year<-y
  df<-as_tibble(df)
  if (is.null(df_all)){
    df_all<-df
  }else{
    df_all<-bind_rows(df_all, df)
  }
}
dbWriteTable(mydb, folder, df_all, overwrite=T)

dbDisconnect(mydb)

