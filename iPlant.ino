#include <FileIO.h> 

// #include <dht11.h>


// dht11 DHT11;

#define DHT11PIN 0
#define SW1PIN   2
#define SW2PIN   3
#define PUMP_PIN_1  4
#define PUMP_PIN_2  5
#define PUMP_PIN_3  6
#define LIGHT_PIN   1
#define TWO_SEC_DELAY 2000
#define FOUR_SEC_DELAY 4000


#include <LiquidCrystal.h>

// LiquidCrystal lcd(7, 8, 3, 4, 5, 6);
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

int backLight = 13;    // pin 13 will control the backlight
byte relayPin[4] = {2,3,4,5};
//D2 -> RELAY1
//D3 -> RELAY2
//D4 -> RELAY3
//D5 -> RELAY4

int week = 3;
int duoA = 75;
int duoB = 25;
int lightHoursOn = 18;
int lightHoursOff = 6;
String startDate = "05-25-16";
int weekNum = 1;

double pH = 6.3;
double ec = 1.9;
double level = 10;
double reslevel = 20;

int pwm6 = 0;
int pwm7 = 0;
int pwm8 = 0;
int pwm9 = 0;

int sw1 = 0;
int sw2 = 0;
String s1 = "ON";
String s2 = "ON";

int tempA = 76;
int tempW = 73;

void setup(){
  Bridge.begin();
  Serial.begin(9600);
  FileSystem.begin();
  
  pinMode(backLight, OUTPUT);
  digitalWrite(backLight, HIGH); // turn backlight on. Replace 'HIGH' with 'LOW' to turn it off.

  // set up switches
   pinMode(SW1PIN, INPUT);
   pinMode(22, OUTPUT);
   digitalWrite(SW1PIN, HIGH);   // turn on pullups
   pinMode(SW2PIN, INPUT);
   digitalWrite(SW2PIN, HIGH);   // turn on pullups
   
  for(int i = 0; i < 4; i++)  pinMode(relayPin[i],OUTPUT);

  lcd.begin(16,2);
  
  lcdPrint("IPlant V 1.70", "Week " + String(week) + "        ", FOUR_SEC_DELAY);
  lcdPrint("Status    ", "A=" + String(duoA) + "mL  B=" + String(duoB) +"mL", FOUR_SEC_DELAY);  
  lcdPrint("LightCycle " + String(lightHoursOn) + "-" + String(lightHoursOff) + "    ", "Started " + startDate + "   ", FOUR_SEC_DELAY);

  lcd.noDisplay();
  delay(500);
  lcd.display();


}

void loop() {
  String dataString;
  dataString += getTimeStamp();
  dataString += " = ";
  // read three sensors and append to the string:
  for (int analogPin = 0; analogPin < 7; analogPin++) {
    int sensor = analogRead(analogPin);
    dataString += String(sensor);
    if (analogPin < 6) {
      dataString += ",";  // separate the values with a comma
    }
  }

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  // The FileSystem card is mounted at the following "/mnt/FileSystema1"
  File dataFile = FileSystem.open("/mnt/sd/datalog.txt", FILE_APPEND);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    SerialUSB.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    SerialUSB.println("error opening datalog.txt");
  }

  delay(15000);
  readSwitches();

 // int tempF = (int)(1.8 * DHT11.temperature + 32);
 // int humid = (int)(DHT11.humidity);
 // lcdPrint("Temp " + String(tempF) + "   Hum " + String(humid), "Temp Water " + String(tempF) + "    ", TWO_SEC_DELAY);
  // if (sw1 == HIGH)  {
    lcdPrint("ph     "  + String(pH) + "    ", "EC  "  + String(ec) + "    ", TWO_SEC_DELAY);
    lcdPrint("Level      "  + String(level) + "    ", "Res       "  + String(level) + "L    ", TWO_SEC_DELAY);
    lcdPrint("pwm6="  + String(pwm6) + "    pwm7=0", "pwm8=0   pwm9=0", TWO_SEC_DELAY);
    lcdPrint("A1=0   A2=0    ", "A3=0    A4=0     ", TWO_SEC_DELAY);
    lcdPrint("A5=0   A6=0    ", "A7=0    A8=0     ", TWO_SEC_DELAY);
    lcdPrint("A9=0   A10=0   ", "A11=0  A12=0     ", TWO_SEC_DELAY);
    lcdPrint("Switch1 = " + s1 + "       ", "Switch2 = " + s2 + "      ", TWO_SEC_DELAY); 
    lcdPrint("IP: 192.168.1.68   ", "Connected    ", TWO_SEC_DELAY);
 // }

  lcdPrint("READ TEMP       ", "DHT11      ", TWO_SEC_DELAY);

  lightTimerOn();

}


void lcdPrint(String firstLine, String secondLine, int delayTime) {
  lcd.setCursor(0, 0);
  lcd.print(firstLine);
  lcd.setCursor(0, 1);
  lcd.print(secondLine);
  delay(delayTime);

}

void pumpTimerOn() {
  digitalWrite(PUMP_PIN_1, HIGH);
  digitalWrite(PUMP_PIN_2, HIGH);
  digitalWrite(PUMP_PIN_3, HIGH);
  lcd.setCursor(0, 1);
  lcd.print("Pump on");
}

void pumpTimerOff() {
  digitalWrite(PUMP_PIN_1, LOW);
  digitalWrite(PUMP_PIN_2, LOW);
  digitalWrite(PUMP_PIN_3, LOW);
  lcd.setCursor(0, 1);
  lcd.print("Pump on");
}

void lightTimerOn() {
  digitalWrite(LIGHT_PIN, HIGH);
  digitalWrite(22, LOW);   // turn on pullups

  lcd.setCursor(0, 1);
  lcd.print("Light on");
}

void lightTimerOff() {
  digitalWrite(LIGHT_PIN, LOW);
    digitalWrite(22, HIGH);   // turn on pullups
  lcd.setCursor(0, 1);
  lcd.print("Light on");
}

void readSwitches()  {
  s1 = "ON";
  sw1 = digitalRead(SW1PIN);
  if (sw1 == HIGH) {
    s1 = "OFF";
  }
  s2 = "ON";
  sw2 = digitalRead(SW2PIN);
  if (sw2 == HIGH) {
    s2 = "OFF";
  }
}


void readTempHumid() {

 // int chk = DHT11.read(DHT11PIN);
 int chk = 1;
  String line1;
  String line2 = "DHT11";
  lcdPrint("Read sensor: ", "DHT11      ", TWO_SEC_DELAY);

  /*
  switch (chk)
  {
    
    case DHTLIB_OK: 
       line1 = "OK"; 
    break;
    case DHTLIB_ERROR_CHECKSUM: 
       line1 = "Checksum error"; 
    break;
    case DHTLIB_ERROR_TIMEOUT: 
       line1 = "Time out error"; 
    break;
    default: 
       line1 = "Unknown error";
    break;
  }
  */
  lcdPrint(line1, line2, TWO_SEC_DELAY); 

//  float myFloat = (float)DHT11.humidity;
  lcdPrint("Humidity (%): ", "DHT11", TWO_SEC_DELAY);

//  myFloat = (float)DHT11.temperature;
  lcdPrint("Temperature (C): ",  "DHT11", TWO_SEC_DELAY);

  lcdPrint("Temperature (F): ", "DHT11", TWO_SEC_DELAY);
//  Serial.println((1.8 *(float)DHT11.temperature + 32), 2);

  Serial.print("Temperature (K): ");
//  Serial.println((DHT11.temperature + 273.15), 2);

  Serial.print("Dew Point (°C): ");
 // Serial.println(dewPoint(DHT11.temperature, DHT11.humidity));

  Serial.print("Dew PointFast (°C): ");
//  Serial.println(dewPointFast(DHT11.temperature, DHT11.humidity));
    delay(2000);
}


// This function return a string with the time stamp
String getTimeStamp() {
  String result;
  Process time;
  // date is a command line utility to get the date and the time
  // in different formats depending on the additional parameter
  time.begin("date");
  time.addParameter("+%D-%T");  // parameters: D for the complete date mm/dd/yy
  //             T for the time hh:mm:ss
  time.run();  // run the command

  // read the output of the command
  while (time.available() > 0) {
    char c = time.read();
    if (c != '\n') {
      result += c;
    }
  }

  return result;
}
