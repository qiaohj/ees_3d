logbase<-"/home/huijieqiao/git/ees_3d_data/SMART_SPECIES/Results_TEST/11198_GOOD_MODERATE_3/11198_GOOD_MODERATE_3"
nb_df<-read.table(sprintf("%s.nb.log", logbase), head=F, sep=",", quote = "\'", stringsAsFactors = F)
head(nb_df)
tail(nb_df)
dim(nb_df)
