library(tidyverse)
library(grid)

f <- file("/dev/cu.usbserial-DA01HLQC", open="r")
data <- data.frame(scan(f, n=60, what = "double", allowEscapes = TRUE, sep = "\n"))
close(f)

colnames(data) <- "all_data"
parsed_data <-  separate(data, all_data, 
                         c("voltage_t", 
                           "temp_c", 
                           "voltage_h", 
                           "humidity", 
                           "humidiry_rel", 
                           "dewpoint", 
                           "clound_base"), 
                         sep = " ")

# remove first row (sometimes has NAs and jumbled columns)
parsed_data <- parsed_data[-1,]

# change all columns to numeric
parsed_data <- map_df(parsed_data, as.numeric)

# add a time column for plotting
parsed_data$time <- seq_along(parsed_data$voltage_t)

# plot dewpoint and temp_c
dew_temp_plot <- ggplot(parsed_data, 
                        aes(x = time, y = temp_c, colour = "Temperature (C)")) +
  geom_line() +
  geom_line(aes(x = time, y = dewpoint, colour = "Dewpoint")) +
  ylim(10, 21) +
  ylab("Temperature (C)") +
  xlab("Time(s)")
 
dew_temp_plot

# plot humidity
hum_plot <- ggplot(parsed_data, 
                   aes(x = time, y = humidiry_rel, colour = "Relative Humidity")) +
  geom_line() +
  ylab("Relative Humidity") +
  xlab("Time(s)")

hum_plot

# overlay plots on same graph (with two-axis)
grid.newpage()
grid.draw(rbind(ggplotGrob(dew_temp_plot), ggplotGrob(hum_plot), size = "last"))
