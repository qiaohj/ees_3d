if (F){
  library("DBI")
  mydb <- dbConnect(RSQLite::SQLite(), sprintf("%s/ISEA3H8/SQLITE/env_Hadley3D.sqlite", base))  
  max_prec<-dbReadTable(mydb, "Debiased_Maximum_Monthly_Precipitation")
  max_temp<-dbReadTable(mydb, "Debiased_Maximum_Monthly_Temperature")
  min_temp<-dbReadTable(mydb, "Debiased_Minimum_Monthly_Temperature")
  dbDisconnect(mydb) 
  head(max_prec)
  max_prec_se<-max_prec%>%dplyr::group_by(year)%>%
    dplyr::summarise(mean=mean(v),
                     sd=sd(v),
                     ci=CI(v)[2]-CI(v)[3])
  max_prec_se$year<-max_prec_se$year*-1
  max_prec_se$type<-"Maximum Monthly Precipitation"
  
  max_temp_se<-max_temp%>%dplyr::group_by(year)%>%
    dplyr::summarise(mean=mean(v),
                     sd=sd(v),
                     ci=CI(v)[2]-CI(v)[3])
  max_temp_se$type<-"Maximum Monthly Temperature"
  
  max_temp_se$year<-max_temp_se$year*-1
  plot(max_temp_se$year, max_temp_se$mean, type="l", col="red")
  
  min_temp_se<-min_temp%>%dplyr::group_by(year)%>%
    dplyr::summarise(mean=mean(v),
                     sd=sd(v),
                     ci=CI(v)[2]-CI(v)[3])
  min_temp_se$type<-"Minimum Monthly Temperature"
  min_temp_se$year<-min_temp_se$year*-1
  lines(min_temp_se$year, min_temp_se$mean, col="blue")
  
  env_se<-bind_rows(bind_rows(max_prec_se, max_temp_se), min_temp_se)
  
  env_df_new<-NULL
  for (var in c("Maximum Monthly Precipitation",
                "Maximum Monthly Temperature",
                "Minimum Monthly Temperature")){
    print(var)
    env_df<-env_se%>%dplyr::filter(type==var)
    
    max_temp_gam<-gam(mean~s(year, k=20),data=env_df)
    #plot(max_temp_gam)
    
    env_df$predicted<-predict(max_temp_gam, env_df)
    
    v<-as.vector(pull(env_df, predicted))
    tp<-turnpoints(v)
    env_df$peak<-tp$peaks
    env_df$pit<-tp$pits
    env_df$direction<-0
    env_df$group<-0
    env_df$length<-0
    direction<-0
    length<-0
    group<-0
    for (year in c(-1200:0)){
      if (pull(env_df[which(env_df$year==year), "peak"])){
        env_df[which(env_df$group==group), "length"]<-length
        direction<- -1
        length<-0
        group<-group+1
      }
      if (pull(env_df[which(env_df$year==year), "pit"])){
        env_df[which(env_df$group==group), "length"]<-length
        direction<- 1
        length<-0
        group<-group+1
      }
      length<-length+1
      env_df[which(env_df$year==year), "direction"]<- direction
      env_df[which(env_df$year==year), "group"]<- group
    }
    env_df[which(env_df$group==group), "length"]<-length
    unique(env_df[, c("group", "length")])
    
    if (is.null(env_df_new)){
      env_df_new<-env_df
    }else{
      env_df_new<-bind_rows(env_df_new, env_df)
    }
    ggplot(env_df, aes(x=year, y=predicted, color=factor(direction)))+geom_point()
  }
  saveRDS(env_df_new, sprintf("%s/Data/env_se.rda", base))
}
library(ggplot2)
env_se<-readRDS(sprintf("%s/Data/env_se.rda", base))

ggplot(env_se, aes(x=year, y=mean, color=factor(type)))+geom_line()

if (F){
  prec<-env_se%>%dplyr::filter(type=="Maximum Monthly Precipitation")
  range(prec$mean)
  min_prec<-min(prec$mean)
  range_prec<-range(prec$mean)[2] - range(prec$mean)[1]
  
}
min_prec<-6
scale<-23
offset<-35


p<-ggplot() +
  geom_line(data=env_se%>%dplyr::filter(type!="Maximum Monthly Precipitation"),
            aes(x=year, y=mean, color=factor(type)), size=2) + 
  geom_line(data=env_se%>%dplyr::filter(type=="Maximum Monthly Precipitation"),
            aes(x=year, y=(mean-min_prec)*scale-offset), size=2, color="black") + 
  #geom_point(data=env_se%>%dplyr::filter(type!="Maximum Monthly Precipitation"),
  #          aes(x=year, y=predicted, group=type, color=factor(direction)), size=0.5) + 
  #geom_point(data=env_se%>%dplyr::filter(type=="Maximum Monthly Precipitation"),
  #          aes(x=year, y=(predicted-min_prec)*scale-offset, color=factor(direction)), size=0.5) + 
  
  scale_y_continuous(
    # Features of the first axis
    name = "Monthly Temperature",
    # Add a second axis and specify its features
    sec.axis = sec_axis(~(.-min_prec)*scale-offset, name="Monthly Precipitation")
  ) + 
  
  theme_bw() +
  ggtitle("Environmental variables")
ggsave(p, filename=sprintf("%s/Figures/env_curves.png", base), width=10, height = 5)
ggsave(p, filename=sprintf("%s/Figures/env_curves.pdf", base), width=10, height = 5)

p<-ggplot() +
  geom_line(data=env_se%>%dplyr::filter(type!="Maximum Monthly Precipitation"),
            aes(x=year, y=mean, color=factor(type)), size=2) + 
  geom_line(data=env_se%>%dplyr::filter(type=="Maximum Monthly Precipitation"),
            aes(x=year, y=(mean-min_prec)*scale-offset), size=2, color="black") + 
  geom_point(data=env_se%>%dplyr::filter(type!="Maximum Monthly Precipitation"),
             aes(x=year, y=predicted, group=type, color=factor(direction)), size=0.5) + 
  geom_point(data=env_se%>%dplyr::filter(type=="Maximum Monthly Precipitation"),
             aes(x=year, y=(predicted-min_prec)*scale-offset, color=factor(direction)), size=0.5) + 
  
  scale_y_continuous(
    # Features of the first axis
    name = "Monthly Temperature",
    # Add a second axis and specify its features
    sec.axis = sec_axis(~(.-min_prec)*scale-offset, name="Monthly Precipitation")
  ) + 
  
  theme_ipsum() +
  ggtitle("Environmental variables")
ggsave(p, filename=sprintf("%s/Figures/env_curves_with_smooth.png", base), width=10, height = 5)
ggsave(p, filename=sprintf("%s/Figures/env_curves_with_smooth.pdf", base), width=10, height = 5)
