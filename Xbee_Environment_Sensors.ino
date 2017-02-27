/*
 Curtis Brett Jan 2017
  ReadAnalogVoltage
  Reads an analog input on pin 0, converts it to voltage, and prints the result to the serial monitor.
  Graphical representation is available using serial plotter (Tools > Serial Plotter menu)
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.
/*****************************************************************
XBee_Serial_Passthrough.ino

Set up a software serial port to pass data between an XBee Shield
and the serial monitor.

Hardware Hookup:
  The XBee Shield makes all of the connections you'll need
  between Arduino and XBee. If you have the shield make
  sure the SWITCH IS IN THE "DLINE" POSITION. That will connect
  the XBee's DOUT and DIN pins to Arduino pins 2 and 3.

*****************************************************************/
// We'll use SoftwareSerial to communicate with the XBee:
#include <SoftwareSerial.h>
// XBee's DOUT (TX) is connected to pin 2 (Arduino's Software RX)
// XBee's DIN (RX) is connected to pin 3 (Arduino's Software TX)
#include <SoftwareSerial.h>
SoftwareSerial XBee(2, 3); //RX, TX

const int numReadings = 20; // size of the array

float readings[numReadings];  // the readings from analog input
float readingsH[numReadings];
int readIndex = 0;          // the index of the current reading
float total = 0;              // the running total
float totalH = 0;
float average = 0;            // the average
float averageH = 0;
const int supplyVoltage = 5;
const int photoPin = 0;
float lightLevel, high = 0, low = 1023;

// the setup routine runs once when you press reset:
void setup() {
  // setup both ports at 9600 bits per second:
  Serial.begin(9600);
  XBee.begin(9600);
  for (int thisReading = 0; thisReading < numReadings; thisReading++){
    readings[thisReading] = 0;
  }
}
  



// the loop routine runs over and over again forever:
void loop() {
  float degreesC, sensorRH, trueRH, TdewPoint, CloudBase;

 // Photoresistor
 lightLevel = analogRead(photoPin);
 float Vout = 5*lightLevel/1024;
int lux=(2500/Vout-500)/10; //https://arduinodiy.wordpress.com/2013/11/03/measuring-light-with-an-arduino/
 

  
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  // read the input on analog pin 0:
  int sensorValueT = analogRead(A3);
  int sensorValueH = analogRead(A1);
  int sensorValueP = analogRead(A0);
  float voltageT = sensorValueT * (supplyVoltage / 1024.0);
  float voltageH = sensorValueH * (supplyVoltage / 1024.0);
  
  // subtract the last reading:
  total = total - readings[readIndex];
  totalH = totalH - readingsH[readIndex];
  // read from the sensor:
  readings[readIndex] = ((voltageT - 0.5) * 100.0) - 5;
  //readingsH[readIndex] = (161.0 * voltageH / supplyVoltage - 25.8) / (1.0546 - (0.0026 * (((voltageT - 0.5) * 100.0)-5)));
  
  degreesC = ((voltageT - 0.5) * 100.0)-5;
  sensorRH = 161.0 * voltageH / supplyVoltage - 25.8;
  trueRH = sensorRH / (1.0546 - (0.0026 * degreesC)); //temperature adjustment
  
  readingsH[readIndex] = sensorRH / (1.0546 - (0.0026 * degreesC));
  
  // add the reading to the total:
  total = total + readings[readIndex];
  totalH = totalH + readingsH[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // calculate the average:
  average = total / numReadings; //smoothed T data
  averageH = totalH / numReadings; //smoothed Humidity data
  
  // Calculate temperature

//   degreesC = ((voltageT - 0.5) * 100.0)-5;

  // trueRH = (161.0 * voltageH / supplyVoltage - 25.8) / (1.0546 - (0.0026 * (((voltageT - 0.5) * 100.0)-5)));
  
  // Calculate Humidity
  // sensorRH = 161.0 * voltageH / supplyVoltage - 25.8;
   //trueRH = sensorRH / (1.0546 - (0.0026 * degreesC)); //temperature adjustment
   TdewPoint = (degreesC) - ((100 - trueRH)/5);
   CloudBase = ((((degreesC - TdewPoint) / 4.5) * 1000) + 50);
  // print out the value you read:

  Serial.println(readingsH[readIndex]);
  XBee.print(voltageT);
  XBee.print(" ");
  XBee.print(degreesC);
  XBee.print(" ");
  XBee.print(average);
  XBee.print(" ");
  XBee.print(voltageH);
  XBee.print(" ");
  XBee.print(sensorRH);
  XBee.print(" ");
  XBee.print(trueRH);
  XBee.print(" ");
  XBee.print(averageH);
  XBee.print(" ");
  XBee.print(TdewPoint);
  XBee.print(" ");
  XBee.print(CloudBase);
  XBee.print(" ");
  XBee.print(lightLevel);
  XBee.print(" ");
  XBee.print(Vout);
  XBee.print(" ");
  XBee.println(lux);
 // XBee.print(tstamp);
 // XBee.println(" ");
  delay(500);
}
