/*This project of self watering system is done for complete House Plantcare
 * Monitor every plant soil moisture
 * Monitor current temperature and air huidity
 * Watering plant on threshold
 * Low level water tank sound alarm
 * 
 * thing to do
 * include ph claibration probe
 * include lcd menu
 * 
 * 
 * 
 * Hardware:
 *Arduino MEGA-2560-R3-ATMEGA16U2-ATMEGA2560-16AU-Board-
 *Sensor Shield V2.0 Board For Arduino Mega2560 R3 ATmega16U2 ATMEL AVR IM
 *LCD2004 lcd 20x4 I2C Crystalfont 
 *16-Channel 5V Relay Shield Module with optocoupler For Arduino
 *I2C IIC RTC DS1307 AT24C32 Real Time Clock Module
 *DC12V Electric Magnet Solenoid Valve Gas Air Water Snuffle Valve Normally Closed
 *12vdc Car Winshield Washer pump
 *DHT11 (temperature and humidity sensor)
 *low level tank water sensor
 *Piedzo buzzer
 * * 
 */

// libraries definition
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "RTClib.h"
#include <SimpleDHT.h>

// pins definition
int plantSensorPin[] = {A0,A1,A2,A3,A4};
int plantActuatorPins[] = {46,47,48,49,50};
int waterlevelPin = A15;
int pumpPin = 45;
int pinDHT11 = 2;
SimpleDHT11 dht11;



// variables
int plantSensorSetPoints[] = {650,650,650,650,650};
int plantSensorValues[] = {0,0,0,0,0};
bool pumpActive = false;
bool plantErrors[] = {false, false, false, false, false};
bool plantActuatorActive[] = {false, false, false, false, false};
int nbPlant = 5;
int wateringDuration = 10000;
unsigned long previousMillis = 0;
unsigned long previousMillisMain = 0;
const long mainInterval = 1000; // Main loop
const long interval = 1000;   // interval at which to blink (milliseconds)
int dateEnable = false;
int plantsEnable = false;
byte temperature = 0;
byte humidity = 0;
int waterlevelsensor = 0;
 
// system messages
const char *string_table[] =
{   
  "  Lyncos & Manitoo",
  " Plants readings...",
  "      Dry soil",
   "     Moist soil",
  "     Soggy soil",
  "The water pump is on",
  "       Manisa",
  "   Watering System",
  "    Please wait!",
   "         by"
};


// objects definition
RTC_DS1307 RTC;
LiquidCrystal_I2C lcd(0x27,20,4);

// SEC = 800
void setup() {

 // serial initialization
  Serial.begin(19200);
  
  // LCD initialization
  lcd.init();          
  lcd.backlight();     // with Backlight
  lcd.clear();         // clearscreen

  lcd.setCursor(0,0);
  lcd.print(string_table[6]); 
  lcd.setCursor(0,1);
  lcd.print(string_table[7]); 
  lcd.setCursor(0,2);
  lcd.print(string_table[9]); 
  lcd.setCursor(0,3);
  lcd.print(string_table[0]);
  delay(3000);
  lcd.clear();
  // Wire initialization
  Wire.begin();
  
  // RTC initialization
  RTC.begin();
  if (!RTC.isrunning()){
    // date and time adjust as the PC computer date and time
    RTC.adjust(DateTime(__DATE__, __TIME__)); 
  }
  

 for ( int i = 0; i < nbPlant; i++)
  {
    pinMode(plantActuatorPins[i], OUTPUT);
    digitalWrite(plantActuatorPins[i], HIGH);
  }
  pinMode(pumpPin, OUTPUT);
  digitalWrite(pumpPin, HIGH);
}


unsigned int mainCounter = 0;
void loop() {
 
  // RTC parameters definition
  DateTime myRTC = RTC.now();
  int H = myRTC.hour();
  int M = myRTC.minute();
  int S = myRTC.second();
  unsigned long currentMillis = millis();
  
 // if low water level: plays the low level alarm
 
  
  pumpActive = false;
  for (int i = 0; i < nbPlant; i++)
  {
    if (plantActuatorActive[i])
    {
      digitalWrite(plantActuatorPins[i], LOW); 
      pumpActive = true;
      digitalWrite(pumpPin, LOW);
       {
         if(waterlevelsensor > 528)
  {
   waterlevelsensor = analogRead(waterlevelPin);
   pumpActive = false;
   digitalWrite(pumpPin, LOW); 
  } 
       }
    }
    else
    {
      digitalWrite(plantActuatorPins[i], HIGH);  
    }
  }
 
  if (pumpActive)
  {
   digitalWrite(pumpPin, LOW);
  }
  else
  {
   digitalWrite(pumpPin, HIGH);
  }
 
  // MAIN LOOP
  if (currentMillis - previousMillisMain >= mainInterval) 
  {
    previousMillisMain = currentMillis;

    mainCounter++;
    if (mainCounter == 5)
    { 
      lcd.clear();
      plantsEnable = false;
      dateEnable = true;
    }
    if (mainCounter == 10)
    {
      lcd.clear();
      plantsEnable = true;
      dateEnable = false;
    }  
    if (mainCounter > 10)
    {
      mainCounter = 0;
    }
  }
  
  if (plantsEnable)
   if (dht11.read(pinDHT11, &temperature, &humidity, NULL )) 
  {
 
      
      lcd.setCursor(0,0);
      lcd.print("P1:");
      lcd.print(plantSensorValues[0]);
      lcd.setCursor(14,0);
      lcd.print("P2:");
      lcd.print(plantSensorValues[1]);
      lcd.setCursor(7,1);
      lcd.print("P3:");
      lcd.print(plantSensorValues[2]);
      lcd.setCursor(0,2);
      lcd.print("P4:");
      lcd.print(plantSensorValues[3]);
      lcd.setCursor(14,2);
      lcd.print("P5:");
      lcd.print(plantSensorValues[4]);
      lcd.setCursor(0,3);  
      lcd.print((int)temperature); lcd.print("*C");
      lcd.setCursor(16,3); 
      lcd.print((int)humidity); lcd.print("%H");    
    }

 
  if (dateEnable)
  {
    RightHour();
  }


// reading sensor every interval and print some debug
  if (currentMillis - previousMillis >= interval) 
  {
    previousMillis = currentMillis;
      
   Serial.println("------- PLANT READINGS ----------");
   for (int thisPlantSensor = 0; thisPlantSensor < nbPlant; thisPlantSensor++)
   {
        plantSensorValues[thisPlantSensor] = analogRead(plantSensorPin[thisPlantSensor]);
        Serial.print("Plant #");
        Serial.print(thisPlantSensor);
        Serial.print(":");
       
        // Set the sensor into error
        if (plantSensorValues[thisPlantSensor] > 1000)
        {
          plantErrors[thisPlantSensor] = true;
        }
        if (plantErrors[thisPlantSensor] == false)
        {
          Serial.print(plantSensorValues[thisPlantSensor]);      
 
          if (plantSensorValues[thisPlantSensor] > plantSensorSetPoints[thisPlantSensor])
          {
            Serial.println("     GIVE ME WATER!!!!!");
            plantActuatorActive[thisPlantSensor] = true;
          }
           else
          {
            Serial.println("no water needed");
            plantActuatorActive[thisPlantSensor] = false;
          }  
 
        }
        else
        {
          Serial.println("Sensor Error");
        }
   }
   Serial.print("temperature ");Serial.print((int)temperature);Serial.print("*C");
   Serial.println("");
   Serial.print("humidity ");Serial.print((int)humidity);Serial.print("%H"); 
   Serial.println("");
   waterlevelsensor = analogRead(waterlevelPin);
   Serial.print("water level ");Serial.print(waterlevelsensor);Serial.print("Test");
   Serial.println("");
   Serial.println("");
   Serial.println("");
 }
}

// Real Time Clock Function
void RightHour()
{
  DateTime Now = RTC.now();
  String clock_date = "  Date: ";
  String clock_hour = "   Time: ";
  
  int _day = Now.day();
  int _month = Now.month();
  int _year = Now.year();
  
  clock_date += fixZero(_day);
  clock_date += "/";
  clock_date += fixZero(_month);
  clock_date += "/";
  clock_date += _year;
  
  int _hour = Now.hour();
  int _minute = Now.minute();
  int _second = Now.second();
  
  clock_hour += fixZero(_hour);
  clock_hour += ":";
  clock_hour += fixZero(_minute);
  clock_hour += ":";
  clock_hour += fixZero(_second);

  lcd.setCursor(0, 0);
  lcd.print(clock_date);
  lcd.setCursor(0, 1);
  lcd.print(clock_hour);
  lcd.setCursor(0,3);
  lcd.print("Reading sensors...");  
  delay(50);
}

String fixZero(int i)
{
  String ret;
  if (i < 10) ret += "0";
  ret += i;
  return ret;
}
