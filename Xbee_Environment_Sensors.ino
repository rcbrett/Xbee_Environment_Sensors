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

*****************************************************************/
// Libraries used:
#include <SoftwareSerial.h> // The SoftwareSerial communicates with the XBee
#include <Wire.h>           // a standard library included with Arduino
#include <SFE_BMP180.h>     // library required to run BMP180 Baro sensor
#include <Adafruit_GPS.h> 
//#include <Ethernet.h>

SoftwareSerial XBee(2, 3);  //RX, TX - XBee's DOUT (TX) is connected to pin 2 (Arduino's Software RX), XBee's DIN (RX) is connected to pin 3 (Arduino's Software TX)
//SoftwareSerial mySerial(3,2);
SFE_BMP180 pressure;        //  create an SFE_BMP180 object, here called "pressure":

#define ALTITUDE 46         // Altitude at 3417 Welwyn St Van BC - need to have this up dated from a GPS

const int supplyVoltage = 5;

// the setup routine runs once when you press reset:
void setup() {
  // setup both ports at 9600 bits per second:
  Serial.begin(9600);
  XBee.begin(9600);
  
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
  float voltageH, voltageP, sensorRH, humidity, TdewPoint, baromin, tempf;
  char status;
  double T,P,p0,a;

  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  // read the input on analog pin 0:
  int sensorValueH = analogRead(A1); // Analouge pin 1 for humidity sensor
  int sensorValueP = analogRead(A0); // Analouge pin 0 for photoresistor
 
  voltageH = sensorValueH * (supplyVoltage / 1023.0); // Calculate Voltage at Humidity sensory  
  voltageP = sensorValueP * (supplyVoltage / 1023.0); // Calculate Voltage from photoresistor

  sensorRH = 161.0 * voltageH / supplyVoltage - 25.81; // https://learn.sparkfun.com/tutorials/hih-4030-humidity-sensor-hookup-guide
  humidity = sensorRH / (1.0546 - (0.00216 * T)); //https://www.sparkfun.com/datasheets/Sensors/Weather/SEN-09569-HIH-4030-datasheet.pdf
  
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

  delay(100);  // Pause for 5 seconds.

  
  //readingsH[readIndex] = sensorRH / (1.0546 - (0.0026 * degreesC));

baromin = (p0*0.0295333727);
tempf = (T * 9.0)/5.0 +32.0;
 
  // Calculations of dew point and cloud base elevation (need to get references)
  TdewPoint = (T) - ((100 - humidity)/5); //averageH is relative humidity average T in C
  int CloudBase = ((((T - TdewPoint) / 2.5) * 1000)); // http://www.csgnetwork.com/estcloudbasecalc.html
// feet above ground level
  
  // Calculate lux from photo-resistor voltage - equation is from website below
  int lux=(2500/voltageP-500)/10; //https://arduinodiy.wordpress.com/2013/11/03/measuring-light-with-an-arduino/
  
  // output to the serial port
  XBee.print(T);
  XBee.print(",");
  XBee.print(humidity);
  XBee.print(",");
  XBee.print(TdewPoint);
  XBee.print(",");
  XBee.print(CloudBase);
  XBee.print(",");
  XBee.print(lux);
  XBee.print(",");
  XBee.print(P/10,3); // absolute pressure in kpa
  XBee.print(",");
  XBee.print(p0/10,3); // pressure at sea level
  XBee.print(",");
  XBee.println(ALTITUDE);
}
  


