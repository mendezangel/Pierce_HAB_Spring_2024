#include <SD.h> //Load SD card library
#include<SPI.h> //Load SPI Library
#include <LiquidCrystal.h> // Load LCD Library
 
#include "Wire.h"    // imports the wire library for talking over I2C 
#include "Adafruit_BMP085.h"  // import the Pressure Sensor Library We are using Version one of Adafruit API for this sensor
Adafruit_BMP085 mySensor;  // create sensor object called mySensor

int rs = 2, enable = 3, d4 = 8, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, enable, d4, d5, d6, d7); // Initialize the LCD library with the numbers of the interface pins

unsigned long startTime = 0; // Variable to store the start time
unsigned long elapsedTime = 0; // Variable to store the elapsed time
boolean running = false; // Variable to track if the stopwatch is running
 
float tempC;  // Variable for holding temp in C
float tempF;  // Variable for holding temp in F
float pressure; // Variable for holding pressure reading
float altitude; // Variable for holding altitude
 
int chipSelect = 4; //chipSelect pin for the SD card Reader
File mySensorData; //Data object you will write your sesnor data to
 
void setup(){
  Serial.begin(9600); // turn on serial monitor
  delay(300); // Wait for newly restarted system to stabilize
  mySensor.begin();   //initialize pressure sensor mySensor
  pinMode(9, INPUT_PULLUP); // Have button connected to this pin to operate
  
  lcd.begin(16, 2); // initialize interface to LCD screen with width and height of display

  Serial.print("Initializing SD card...");
   
  pinMode(10, OUTPUT); //Must declare 10 an output and reserve it (for SD card)

  if (!SD.begin(chipSelect)) {
    Serial.println("Initialization failed!");
    return; // If unable to initialize SD card terminate execution
  }

  Serial.println("Initialization done.");
  lcd.clear();
  lcd.print("SD card OK");
  delay(2000);
  lcd.clear();


// Attempt to open/create test.csv file on SD card and write to it.
  mySensorData = SD.open("test.csv", FILE_WRITE);
  if (mySensorData) {
    Serial.print("Writing to test.txt...");
    mySensorData.println("Testing!");
    mySensorData.close();

    Serial.println("Done.");
  } else {
    Serial.println("Error opening test.txt");
  }
}
 
void loop() {
  tempC = mySensor.readTemperature(); //  Read Temperature from BMP180
  tempF = tempC*1.8 + 32.; // Convert degrees C to F
  pressure = mySensor.readPressure(); // Read Pressure
  altitude = mySensor.readAltitude(); // Read altitude

  if (digitalRead(9) == LOW) { // When button is pressed...
    if (!running) {
      startTime = millis(); // Start the stopwatch
      running = true;
    } else {
      elapsedTime = millis() - startTime; // Stop the stopwatch and calculate elapsed time
      running = false;
    }
    delay(200); // Debouncing delay to avoid multiple readings from a single press
  }

  if (running) {
    elapsedTime = millis() - startTime; // Update elapsed time continuously while running
    lcd.setCursor(0, 1); // Set the cursor to the first column of the second row
    lcd.print("Running...  ");

    // opening test.csv file on SD card and writing data to it
    mySensorData = SD.open("test.csv", FILE_WRITE);
    if (mySensorData) {
      String timeStamp = parseTime(elapsedTime);

      // Printing results to serial monitor
      Serial.print("The Temp is: ");
      Serial.print(tempF);
      Serial.println(" degrees F");
      Serial.print("The Pressure is: ");
      Serial.print(pressure);
      Serial.println(" Pa.");
      Serial.println("");
      Serial.print("Altitude: ");
      Serial.println(altitude);

      // writing data to card 
      mySensorData.print(tempF);
      mySensorData.print("F");
      mySensorData.print(",");
      mySensorData.print(pressure);
      mySensorData.print("Pa");
      mySensorData.print(",");
      mySensorData.print(altitude);
      mySensorData.print("m");
      mySensorData.print(",");
      mySensorData.print(timeStamp);
      mySensorData.println("s");
      mySensorData.close();
      } else {
        Serial.println("Error opening SD file");
      }
    
  } else {
    lcd.setCursor(0, 1); // Set the cursor to the first column of the second row
    lcd.print("Stopped.   ");
  }
   
  

  // Update the elapsed time on the LCD
  lcd.setCursor(0, 0); // Set cursor position
  lcd.print(elapsedTime / 3600000); // Print hours
  lcd.print(":");
  lcd.print((elapsedTime / 60000) % 60); // Print minutes
  lcd.print(":");
  lcd.print((elapsedTime / 1000) % 60); // Print seconds
  lcd.print(":");
  lcd.print(elapsedTime % 60); // Print miliseconds
//  delay(500);
}

// Method that parses time and returns it as a string
String parseTime(unsigned long elapsedTime) {
  String hours = String(elapsedTime / 3600000);
  String minutes = String((elapsedTime / 60000) % 60);
  String seconds = String((elapsedTime / 1000) % 60);
  String milliseconds = String(elapsedTime % 60);

  String timeStamp = String(hours + ":" + minutes + ":" + seconds + ":" + milliseconds);

  return timeStamp;
}
