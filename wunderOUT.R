## Read Envirnoment data from Serial Port and out put data to a csv

library(tidyverse)
library(grid)
library(purrr)
#library(RCurl)
#library(curl)
library(httr)


f <- file("/dev/cu.usbserial-DA01HLQC", open="r")
data <- (scan(f, n=1, what = "double", allowEscapes = TRUE, sep = "\n"))
close(f)
#write.table(data, "mydata.txt", sep="\n", col.names = FALSE, row.names = FALSE)
POST(data)



#print(data,row.names = FALSE, col.names = FALSE)
# fileConn<-file("output.txt")
# tmp <- sapply(data, as.character) # a character vector
# writeLines(c(tmp), fileConn)
# close(fileConn)