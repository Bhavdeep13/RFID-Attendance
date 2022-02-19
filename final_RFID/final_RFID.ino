/**
 * Owner --> Bhavdeep Singh Krishnawat

 * Note --> Dont forget to use 330 ohm resistance between the MISO pin of SD card Module 
 * connections for Arduino Uno
 RFID -->  
    SDA --> pin 10 
    SCK --> pin 13
    MOSI --> pin 11
    MISO --> pin 12
    IRQ --> unconnected
    GND --> GND
    RST --> pin 9
    3.3V --> 3.3 V

 SD card Module -->
    CS --> pin 4
    SCK --> pin 13
    MOSI --> pin 11
    MISO --> pin 12
    VCC --> +5V
    GND --> GND

 RTC Module (DS1307)
    SCL --> A5
    SDA --> A5
    VCC --> +5V
    GND --> GND

 LCD -->
  16x2
  ` use 10 k potentiometer for adjusting brighteness
    Connect as given in fritzing circuit diagram
    VSS --> +5V
    VDD --> GND
    VO --> potentiometer or GND
    RS --> pin 3
    RW --> pin 2
    D0 --> unconnected
    D1 --> unconnected
    D2 --> unconnected
    D3 --> unconnected
    D4 --> A0
    D5 --> A1
    D6 --> A2
    D7 --> A3
    A --> +5V
    K --> GND


Note dont forget to add library 
MFRC522.h
SPI.h
SD.h
RTClib.h

  BUZZER --> 5
  LED RED --> 6
  LED GREEN --> 7
 **/

#include <LiquidCrystal.h>
LiquidCrystal lcd(3, 2, A0, A1, A2, A3);//LCD CONNECTION PINS

 
#include <MFRC522.h> // for the RFID
#include <SPI.h> // for the RFID and SD card module
#include <SD.h> // for the SD card
#include <RTClib.h> // for the RTC
 
// define pins for RFID
#define CS_RFID 10
#define RST_RFID 9
// define select pin for SD card module
#define CS_SD 4 
 
// Create a file to store the data
File myFile;
 
// Instance of the class for RFID
MFRC522 rfid(CS_RFID, RST_RFID); 
 
// Variable to hold the tag's UID
String uidString;
 
// Instance of the class for RTC
RTC_DS1307 rtc;
 
// Define check in time
const int checkInHour = 9;
const int checkInMinute = 5;
 
//Variable to hold user check in
int userCheckInHour;
int userCheckInMinute;
 
// Pins for LEDs and buzzer
const int redLED = 6;
const int greenLED = 7;
const int buzzer = 5;
 
void setup() {
  
  // Set LEDs and buzzer as outputs
  pinMode(redLED, OUTPUT);  
  pinMode(greenLED, OUTPUT);
  pinMode(buzzer, OUTPUT);
  
  // Init Serial port
  Serial.begin(9600);
  lcd.begin(16,2);
  while(!Serial); // for Leonardo/Micro/Zero
  
  // Init SPI bus
  SPI.begin(); 
  // Init MFRC522 
  rfid.PCD_Init(); 
 
  // Setup for the SD card
  Serial.print("Initializing SD card...");
  lcd.print("Initializing ");
  lcd.setCursor(0, 1);
  lcd.print("SD card...");
  delay(3000);
  lcd.clear();
  if(!SD.begin(CS_SD)) {
    Serial.println("initialization failed!");
    lcd.print("Initializing ");
    lcd.setCursor(0, 1);
    lcd.print("failed!");
    return;
  }
  Serial.println("initialization done.");
  lcd.print("Initialization ");
  lcd.setCursor(0, 1);
  lcd.print("Done...");
  delay(2000);
  lcd.clear();
 
  // Setup for the RTC  
  if(!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    lcd.clear();
    lcd.print("Couldn't find");
    lcd.setCursor(0, 1);
    lcd.print("RTC clock");
    lcd.clear();
    
    while(1);
  }
  else {
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  if(!rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    lcd.clear();
    lcd.print("RTC Not Running!");
  }
}
 
void loop() {
  digitalWrite(redLED, HIGH);
  //look for new cards
    DateTime now = rtc.now();
    lcd.clear();
    lcd.print(now.year(), DEC);
    lcd.print(':');
    lcd.print(now.month(), DEC);
    lcd.print(':');
    lcd.print(now.day(), DEC);
    lcd.print(' ');
    lcd.setCursor(11, 0);
    lcd.print(now.hour(), DEC);
    lcd.print(':');
    lcd.print(now.minute(), DEC);
    lcd.setCursor(0, 1);
    lcd.print("Put RFID to Scan");
  delay(3000);
  lcd.clear();
  if(rfid.PICC_IsNewCardPresent()) {
    readRFID();
    logCard();
    //verifyCheckIn();
  }
  delay(10);
}
 
void readRFID() {
  rfid.PICC_ReadCardSerial();
  lcd.clear();
  Serial.print("Tag UID: ");
  lcd.print("Tag UID: ");
  uidString = String(rfid.uid.uidByte[0]) + " " + String(rfid.uid.uidByte[1]) + " " + 
    String(rfid.uid.uidByte[2]) + " " + String(rfid.uid.uidByte[3]);
  Serial.println(uidString);
  lcd.setCursor(0, 1);
  lcd.print(uidString);
  delay(2000);
 
  // Sound the buzzer when a card is read
  digitalWrite(redLED, LOW);
  tone(buzzer, 3000);
  digitalWrite(greenLED, HIGH); 
  delay(200);        
  noTone(buzzer);
  
  delay(200);
}
 
void logCard() {
  // Enables SD card chip select pin
  digitalWrite(CS_SD,LOW);
  
  // Open file
  myFile=SD.open("DATA.txt", FILE_WRITE);
 
  // If the file opened ok, write to it
  if (myFile) {
    Serial.println("File opened ok");
    lcd.clear();
    lcd.print("File opened ok");
    delay(2000);
    myFile.print(uidString);
    myFile.print(", ");   
    
    // Save time on SD card
    DateTime now = rtc.now();
    myFile.print(now.year(), DEC);
    myFile.print('/');
    myFile.print(now.month(), DEC);
    myFile.print('/');
    myFile.print(now.day(), DEC);
    myFile.print(',');
    myFile.print(now.hour(), DEC);
    myFile.print(':');
    myFile.println(now.minute(), DEC);
    
    // Print time on Serial monitor
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.println(now.minute(), DEC);
    Serial.println("sucessfully written on SD card");
    
 
    lcd.clear();
    lcd.print(now.year(), DEC);
    lcd.print(':');
    lcd.print(now.month(), DEC);
    lcd.print(':');
    lcd.print(now.day(), DEC);
    lcd.print(' ');
    lcd.setCursor(11, 0);
    lcd.print(now.hour(), DEC);
    lcd.print(':');
    lcd.print(now.minute(), DEC);
    lcd.setCursor(0, 1);
    lcd.print("Written on SD...");
    delay(2000);
    lcd.clear();
    lcd.print("Welcome!");
    delay(2000);
    lcd.clear();
    digitalWrite(greenLED, LOW);
    digitalWrite(redLED, HIGH);
    
    myFile.close();
 
    // Save check in time;
    userCheckInHour = now.hour();
    userCheckInMinute = now.minute();
  }
  else {
    
    Serial.println("error opening data.txt");  
    lcd.clear();
    lcd.print("error opening data.txt");
  }
  // Disables SD card chip select pin  
  digitalWrite(CS_SD,HIGH);
}
