library(rgdal)
library(raster)
shape <- readOGR(dsn = "/Users/huijieqiao/git/DGGRID/examples/isea3hGen/outputfiles", layer = "isea3h8")
shape@data$v<-NA
i=858
r<-raster("/Users/huijieqiao/workspace_cpp/EES_Test/M1000/mask.tif")
plor(r)
for (i in c(1:length(shape@polygons))){
  print(paste(i, length(shape@polygons)))
  p<-shape@polygons[[i]]@Polygons[[1]]
  ps = Polygons(list(p),1)
  sps = SpatialPolygons(list(ps))
  #plot(sps)
  
  r_c<-crop(r, sps)
  v<-mean(values(r_c), na.rm=T)
  if (!is.nan(v)){
    shape@data[i, "v"]<-v
  }
}
shape@polygons<-subset(shape@polygons, !is.na(shape@data$v))
shape@data<-subset(shape@data, !is.na(shape@data$v))
writeOGR(shape, dsn = "/Users/huijieqiao/git/DGGRID/examples/isea3hGen/outputfiles", 
         layer = "isea3h8_v_na", driver="ESRI Shapefile", overwrite_layer=T)


shape <- readOGR(dsn = "/Users/huijieqiao/git/DGGRID/examples/isea3hGen/outputfiles", layer = "isea3h8_v_na")

shape@data$v
plot(shape, col=NA, fill=shape$v)
plot(r_c, add=T)

shape@data$v<-extract(r, shape@coords)
plot(shape, pch=".", col=shape@data$v)
plot(shape)
plot(r, add=T)
