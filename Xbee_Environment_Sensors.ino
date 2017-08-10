/*
 Curtis Brett Jan 2017
  ReadAnalogVoltage
  Reads an analog input on pin 0, converts it to voltage, and prints the result to the serial monitor.
 
Set up a software serial port to pass data between an XBee Shield
and the serial monitor.

Hardware Hookup:
  The XBee Shield makes all of the connections you'll need
  between Arduino and XBee. If you have the shield make
  sure the SWITCH IS IN THE "DLINE" POSITION. That will connect
  the XBee's DOUT and DIN pins to Arduino pins 2 and 3.

smoothing algorithm adapted from https://www.arduino.cc/en/Tutorial/Smoothing

*****************************************************************/
// Libraries used:
#include <SoftwareSerial.h> // The SoftwareSerial communicates with the XBee
#include <Wire.h>           // a standard library included with Arduino
#include <SFE_BMP180.h>     // library required to run BMP180 Baro sensor
#include <Adafruit_GPS.h> 
#include <Ethernet.h>
#include <SPI.h>
#include <WiFi.h>
WiFiClient client;



SoftwareSerial XBee(2, 3);  //RX, TX - XBee's DOUT (TX) is connected to pin 2 (Arduino's Software RX), XBee's DIN (RX) is connected to pin 3 (Arduino's Software TX)
//SoftwareSerial mySerial(3,2);
SFE_BMP180 pressure;        //  create an SFE_BMP180 object, here called "pressure":

#define ALTITUDE 46         // Altitude at 3417 Welwyn St Van BC - need to have this up dated from a GPS

const int supplyVoltage = 5;
const int numReadings = 20; // number of readings to average measurements by

float tempC_readings[numReadings];
float humidity_readings[numReadings];
int readIndex = 0;
float totalTC = 0;
float averageTC = 0;
float totalH = 0;
float averageH = 0;


///////////////Weather//////////////////////// 
char server [] = "rtupdate.wunderground.com";
//char server [] = "weatherstation.wunderground.com";  
//char WEBPAGE [] = "GET /weatherstation/updateweatherstation.php";
char WEBPAGE [] = "https://weatherstation.wunderground.com/weatherstation/updateweatherstation.php?";
char ID [] = "IVANCOUV35";
char PASSWORD [] = "mqj7772g";

// the setup routine runs once when you press reset:
void setup() {
  // setup both ports at 9600 bits per second:
  Serial.begin(9600);
  XBee.begin(9600);
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    tempC_readings[thisReading] = 0;
  }
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    humidity_readings[thisReading] = 0;
  }
  
  // Initialize the sensor (it is important to get calibration values stored on the device).
  if (pressure.begin())
    Serial.println("BMP180 init success");
  else
  {
    // Oops, something went wrong, this is usually a connection problem,
    // see the comments at the top of this sketch for the proper connections.
    Serial.println("BMP180 init fail\n\n");
    while(1); // Pause forever.
  }

}
  


void loop() {
  float voltageH, voltageP, sensorRH, humidity, TdewPoint, baromin, tempf, dewpointf;
  char status;
  double T,P,p0,a;

  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  // read the input on analog pin 0:
int sensorValueH = analogRead(A1); // Analouge pin 1 for humidity sensor
int sensorValueP = analogRead(A0); // Analouge pin 0 for photoresistor

totalTC = totalTC - tempC_readings[readIndex];
totalH = totalH - humidity_readings[readIndex];
  
  voltageH = sensorValueH * (supplyVoltage / 1023.0); // Calculate Voltage at Humidity sensory  
  voltageP = sensorValueP * (supplyVoltage / 1023.0); // Calculate Voltage from photoresistor

  sensorRH = 161.0 * voltageH / supplyVoltage - 25.81; // https://learn.sparkfun.com/tutorials/hih-4030-humidity-sensor-hookup-guide
  humidity = sensorRH / (1.0546 - (0.00216 * averageTC)); //https://www.sparkfun.com/datasheets/Sensors/Weather/SEN-09569-HIH-4030-datasheet.pdf
  
  status = pressure.startTemperature();
  if (status != 0)
  {
    // Wait for the measurement to complete:
    delay(status);
    // Retrieve the completed temperature measurement:
    // Note that the measurement is stored in the variable T.
    // Function returns 1 if successful, 0 if failure.

    status = pressure.getTemperature(T);
    if (status != 0)
    {
   // Start a pressure measurement:
      // The parameter is the oversampling setting, from 0 to 3 (highest res, longest wait).
      // If request is successful, the number of ms to wait is returned.
      // If request is unsuccessful, 0 is returned.

      status = pressure.startPressure(3);
      if (status != 0)
      {
        // Wait for the measurement to complete:
        delay(status);

        // Retrieve the completed pressure measurement:
        // Note that the measurement is stored in the variable P.
        // Note also that the function requires the previous temperature measurement (T).
        // (If temperature is stable, you can do one temperature measurement for a number of pressure measurements.)
        // Function returns 1 if successful, 0 if failure.

        status = pressure.getPressure(P,T);
        if (status != 0)
        {
          // Print out the measurement:
//          Serial.print("absolute pressure: ");
//          Serial.print(P,2);
//          Serial.print(" mb, ");
//          Serial.print(P*0.0295333727,2);
//          Serial.println(" inHg");

          // The pressure sensor returns abolute pressure, which varies with altitude.
          // To remove the effects of altitude, use the sealevel function and your current altitude.
          // This number is commonly used in weather reports.
          // Parameters: P = absolute pressure in mb, ALTITUDE = current altitude in m.
          // Result: p0 = sea-level compensated pressure in mb

          p0 = pressure.sealevel(P,ALTITUDE); // 
//          Serial.print("relative (sea-level) pressure: ");
//          Serial.print(p0,2);
//          Serial.print(" mb, ");
//          Serial.print(p0/10,3);
//          Serial.print(" kPa, ");
//          Serial.print(p0*0.0295333727,2);
//          Serial.println(" inHg");

        }
        else Serial.println("error retrieving pressure measurement\n");
      }
      else Serial.println("error starting pressure measurement\n");
    }
    else Serial.println("error retrieving temperature measurement\n");
  }
  else Serial.println("error starting temperature measurement\n");

  delay(1000);  // Pause for 5 seconds.

  
  //readingsH[readIndex] = sensorRH / (1.0546 - (0.0026 * degreesC));

tempC_readings[readIndex] = T;
humidity_readings[readIndex] = humidity;
totalTC = totalTC + tempC_readings[readIndex];
totalH = totalH + humidity_readings[readIndex];
readIndex = readIndex +1;

if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }
 
averageTC = totalTC / numReadings;
averageH = totalH / numReadings;

baromin = (p0*0.0295333727);
tempf = (averageTC * 9.0)/5.0 +32.0;



Serial.print(averageTC);
Serial.print(" ");
Serial.print(T);
Serial.print(" ");
Serial.print(averageH);
Serial.print(" ");
Serial.println(humidity);


  // Calculations of dew point and cloud base elevation (need to get references)
  TdewPoint = (averageTC) - ((100 - averageH)/5); //averageH is relative humidity average T in C
  dewpointf = (TdewPoint * 9.0)/5.0 +32.0;
  int CloudBase = ((((averageTC - TdewPoint) / 2.5) * 1000)); // http://www.csgnetwork.com/estcloudbasecalc.html
// feet above ground level
  
  // Calculate lux from photo-resistor voltage - equation is from website below
  int lux=(2500/voltageP-500)/10; //https://arduinodiy.wordpress.com/2013/11/03/measuring-light-with-an-arduino/
  
  // output to the serial port
//  Serial.print(averageTC);
//  Serial.print(",");
////  Serial.print(tempf);
////  Serial.print(",");
//  Serial.print(humidity);
//  Serial.print(",");
//  Serial.print(TdewPoint);
//  Serial.print(",");
//  Serial.print(dewpointf);
//  Serial.print(",");
//  Serial.print(CloudBase);
//  Serial.print(",");
//  Serial.print(lux);
//  Serial.print(",");
//  Serial.print(P*0.0295333727,2); // inHg
//  Serial.print(",");
//  Serial.print(P/10,3); // absolute pressure in kpa
//  Serial.print(",");
//  Serial.print(p0/10,3); // pressure at sea level
//  Serial.print(",");
//  Serial.println(ALTITUDE);
//// 
   
// Send data to serial port to be read into R script 
     XBee.print(WEBPAGE); 
     XBee.print("ID=");
     XBee.print(ID);
     XBee.print("&PASSWORD=");
     XBee.print(PASSWORD);
     XBee.print("&dateutc=");
     XBee.print("now");    
     XBee.print("&tempf=");
     XBee.print(tempf);
     XBee.print("&baromin=");
     XBee.print(p0*0.0295333727,2);
     XBee.print("&dewptf=");
     XBee.print(dewpointf);
     XBee.print("&humidity=");
     XBee.print(averageH);
     XBee.println("&softwaretype=Arduino%20UNO%20version1&action=updateraw&realtime=1&rtfreq=30");   //Using Rapid Fire, sending data 1time every 30sec
   
   delay(200);  
}

