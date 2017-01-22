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

// the setup routine runs once when you press reset:
void setup() {
  // setup both ports at 9600 bits per second:
  Serial.begin(9600);
  XBee.begin(9600);
}
  const int supplyVoltage = 5;
// the loop routine runs over and over again forever:
void loop() {
  float degreesC, sensorRH, trueRH, TdewPoint, CloudBase;

  // read the input on analog pin 0:
  int sensorValueT = analogRead(A3);
  int sensorValueH = analogRead(A1);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float voltageT = sensorValueT * (supplyVoltage / 1024.0);
  float voltageH = sensorValueH * (supplyVoltage / 1024.0);
  // Calculate temperature
   degreesC = ((voltageT - 0.5) * 100.0)-5;
  // Calculate Humidity
   sensorRH = 161.0 * voltageH / supplyVoltage - 25.8;
   trueRH = sensorRH / (1.0546 - (0.0026 * degreesC)); //temperature adjustment
   TdewPoint = (degreesC) - ((100 - trueRH)/5);
   CloudBase = ((((degreesC - TdewPoint) / 4.5) * 1000) + 50);
  // print out the value you read:
  XBee.print(voltageT);
  XBee.print(" ");
  XBee.print(degreesC);
  XBee.print(" ");
  XBee.print(voltageH);
  XBee.print(" ");
  XBee.print(sensorRH);
  XBee.print(" ");
  XBee.print(trueRH);
  XBee.print(" ");
  XBee.print(TdewPoint);
  XBee.print(" ");
  XBee.print(CloudBase);
  XBee.println(" ");
  delay(2000);
}
