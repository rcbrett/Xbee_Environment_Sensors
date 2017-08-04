## Real Time Serial Read from Sensor
# test

library(tidyverse)
library(grid)
library(purrr)
#arduino.data  = data.frame(read.csv("test.csv", header = FALSE, sep = ","))  # import diamond count data

f <- file("/dev/cu.usbserial-DA01HLQC", open="r")
data <- data.frame(scan(f, n=50, what = "double", allowEscapes = TRUE, sep = "\n"))
close(f)

colnames(data) <- "all_data"
parsed_data <-  separate(data, all_data, 
                            c("Temperature_C",
                             "Humidity_pct",
                             "Dew_Point_C",
                             "CouldBase_ft",
                             "lux",
                             "Station_Pressure_kPa",
                             "Pressure_sea_level_kPa",
                             "Altitude"),
                              sep = ",")
                   
# remove first row (sometimes has NAs and jumbled columns)
parsed_data <- parsed_data[-1,]

# change all columns to numeric
parsed_data <- map(parsed_data, as.numeric) %>% as.data.frame()

means = data.frame(colMeans(parsed_data, na.rm = FALSE, dims = 1))
# plot(arduino.data$Temperature_C, arduino.data$Station_Pressure_kPa)

write.table(means, "~/Documents/RCODE/SerialRead/Xbee_Environment_Sensors/means.csv", sep="\t")


# jpeg('rplot.jpg')
# plot(arduino.data$Temperature_C, arduino.data$Station_Pressure_kPa)
# dev.off()
# # # add a time column for plotting
# # parsed_data$time <- seq_along(parsed_data$voltage_t)
# # 
# # # plot dewpoint and temp_c
# # dew_temp_plot <- ggplot(parsed_data, 
# #                         aes(x = time, y = temp_c, colour = "Temperature (C)")) +
# #   geom_line() +
# #   geom_line(aes(x = time, y = temp_smooth, colour = "Smoothed Temp")) +
# #   ylim(17, 25) +
# #   ylab("Temperature (C)") +
# #   xlab("Time(s)")
# #  
# # dew_temp_plot
# # 
# # # plot humidity
# # hum_plot <- ggplot(parsed_data, 
# #                    aes(x = time, y = humidiry_rel, colour = "Relative Humidity")) +
# #   geom_line() +
# #   geom_line(aes(x = time, y = smoothed_humidity, colour = "Smoothed Humidity")) +
# #   ylab("Relative Humidity %") +
# #   #ylim(60, 65) +
#   xlab("Time(s)")
# 
# hum_plot
# 
# # overlay plots on same graph (with two-axis)
# grid.newpage()
# grid.draw(rbind(ggplotGrob(dew_temp_plot), ggplotGrob(hum_plot), size = "last"))
