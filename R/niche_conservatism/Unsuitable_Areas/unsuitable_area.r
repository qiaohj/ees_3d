library("dplyr")
library("DBI")
library("phytools")
library("tidyr")
library("data.table")
library("tibble")
library("raster")
library("sp")
library("ggplot2")
library("RColorBrewer")


base<-"/home/huijieqiao/git/ees_3d_data/niche_conservatism"
db_base<-"/home/huijieqiao/git/ees_3d_data/SMART_SPECIES"
base2<-"/media/huijieqiao/Butterfly/SMART_SPECIES"


#the rda file was generated by global_id_lonlat.r
mask_df<-readRDS(sprintf("%s/Data/ENV/mask_df.rda", base))
mask_df$Y<-mask_df$Y*-1
mask_df$TEMP_RANGE<-mask_df$MAX_TEMP-mask_df$MIN_TEMP
hist(mask_df$TEMP_RANGE)
BROAD<-60
NARROW<-40
all_point<-nrow(mask_df%>%dplyr::filter(Y==0))
mask_df$LAT_ROUND<-round(mask_df$lat)
N_LAT<-mask_df%>%dplyr::filter(Y==0)%>%dplyr::group_by(LAT_ROUND)%>%dplyr::summarise(N=n())
mask_df_unsuitable<-mask_df%>%dplyr::filter(TEMP_RANGE>BROAD)
mask_df_unsuitable$LAT_ROUND<-round(mask_df_unsuitable$lat)
mask_df_unsuitable_LAT<-mask_df_unsuitable%>%dplyr::group_by(Y, LAT_ROUND)%>%
  dplyr::summarise(N=n())
mask_df_unsuitable_LAT_SE<-mask_df_unsuitable_LAT%>%dplyr::ungroup()%>%
  dplyr::group_by(LAT_ROUND)%>%
  dplyr::summarise(MEAN=mean(N),
                   SD=sd(N),
                   NB="BROAD")

mask_df_unsuitable_N<-mask_df_unsuitable%>%dplyr::group_by(Y)%>%
  dplyr::summarise(N=n(),
                   NB="BROAD")

mask_df_unsuitable<-mask_df%>%dplyr::filter(TEMP_RANGE>NARROW)
mask_df_unsuitable$LAT_ROUND<-round(mask_df_unsuitable$lat)
mask_df_unsuitable_LAT<-mask_df_unsuitable%>%dplyr::group_by(Y, LAT_ROUND)%>%
  dplyr::summarise(N=n())
mask_df_unsuitable_LAT_SE<-bind_rows(mask_df_unsuitable_LAT_SE,
                                     mask_df_unsuitable_LAT%>%dplyr::ungroup()%>%
  dplyr::group_by(LAT_ROUND)%>%
  dplyr::summarise(MEAN=mean(N),
                   SD=sd(N),
                   NB="NARROW"))
mask_df_unsuitable_N<-bind_rows(mask_df_unsuitable_N,
                                mask_df_unsuitable%>%dplyr::group_by(Y)%>%
                                  dplyr::summarise(N=n(),
                                                   NB="NARROW"))

colors<-brewer.pal(8, "Dark2")
nb_color<-colors[c(1,3)]
p<-ggplot(mask_df_unsuitable_N, aes(x=Y, y=N/all_point, color=factor(NB)))+
  geom_line()+
  scale_color_manual(values=nb_color)+
  theme_bw()
p
ggsave(p, file=sprintf("%s/Figures/unsuitable_area.png", base))

mask_df_unsuitable_LAT_SE_ALL<-left_join(mask_df_unsuitable_LAT_SE, N_LAT, by="LAT_ROUND")
p<-ggplot(mask_df_unsuitable_LAT_SE_ALL, aes(x=LAT_ROUND, y=MEAN, color=factor(NB)))+
  geom_line()+
  geom_errorbar(aes(ymin=MEAN-SD, ymax=MEAN+SD))+
  scale_color_manual(values=nb_color)+
  theme_bw()
p
ggsave(p, file=sprintf("%s/Figures/unsuitable_area_lat.png", base))

p<-ggplot(mask_df_unsuitable_LAT_SE_ALL, aes(x=LAT_ROUND, y=MEAN/N, color=factor(NB)))+
  geom_line()+
  geom_errorbar(aes(ymin=(MEAN-SD)/N, ymax=(MEAN+SD)/N))+
  scale_color_manual(values=nb_color)+
  theme_bw()
p
ggsave(p, file=sprintf("%s/Figures/unsuitable_area_lat_propotion.png", base))
