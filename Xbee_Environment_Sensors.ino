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
const int sensorPin = 0;
const int ledPin = 10;

// set global variables for the light level:

float lightLevel, high = 0, low = 1023;
const int voltpin = 0;
 

// the setup routine runs once when you press reset:
void setup() {
  // setup both ports at 9600 bits per second:
  Serial.begin(9600);
  XBee.begin(9600);
  pinMode(ledPin, OUTPUT);
  for (int thisReading = 0; thisReading < numReadings; thisReading++){
    readings[thisReading] = 0;
  }
}
  



// the loop routine runs over and over again forever:
void loop() {
  float degreesC, sensorRH, trueRH, TdewPoint, CloudBase;

 // Photoresistor
 lightLevel = analogRead(sensorPin);
 Serial.println(4.65*lightLevel/1023);
  manualTune();  // manually change the range from light to dark
  //autoTune();  // have the Arduino do the work for us!
  analogWrite(ledPin, lightLevel);

  
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
  XBee.println(" ");
 // XBee.print(tstamp);
 // XBee.println(" ");
  delay(500);
}

void manualTune()
{
  // As we mentioned above, the light-sensing circuit we built
  // won't have a range all the way from 0 to 1023. It will likely
  // be more like 300 (dark) to 800 (light). If you run this sketch
  // as-is, the LED won't fully turn off, even in the dark.
  
  // You can accommodate the reduced range by manually 
  // tweaking the "from" range numbers in the map() function.
  // Here we're using the full range of 0 to 1023.
  // Try manually changing this to a smaller range (300 to 800
  // is a good guess), and try it out again. If the LED doesn't
  // go completely out, make the low number larger. If the LED
  // is always too bright, make the high number smaller.

  // Remember you're JUST changing the 0, 1023 in the line below!

  lightLevel = map(lightLevel, 600, 800, 0, 255);
  lightLevel = constrain(lightLevel, 0, 255);

  // Now we'll return to the main loop(), and send lightLevel
  // to the LED.
} 


void autoTune()
{
  // As we mentioned above, the light-sensing circuit we built
  // won't have a range all the way from 0 to 1023. It will likely
  // be more like 300 (dark) to 800 (light).
  
  // In the manualTune() function above, you need to repeatedly
  // change the values and try the program again until it works.
  // But why should you have to do that work? You've got a
  // computer in your hands that can figure things out for itself!

  // In this function, the Arduino will keep track of the highest
  // and lowest values that we're reading from analogRead().

  // If you look at the top of the sketch, you'll see that we've
  // initialized "low" to be 1023. We'll save anything we read
  // that's lower than that:
  
  if (lightLevel < low)
  {
    low = lightLevel;
  }

  // We also initialized "high" to be 0. We'll save anything
  // we read that's higher than that:
  
  if (lightLevel > high)
  {
    high = lightLevel;
  }
  
  // Once we have the highest and lowest values, we can stick them
  // directly into the map() function. No manual tweaking needed!
  
  // One trick we'll do is to add a small offset to low and high,
  // to ensure that the LED is fully-off and fully-on at the limits
  // (otherwise it might flicker a little bit).
  
  lightLevel = map(lightLevel, low+30, high-30, 0, 255);
  lightLevel = constrain(lightLevel, 0, 255);
  
  // Now we'll return to the main loop(), and send lightLevel
  // to the LED.
}


