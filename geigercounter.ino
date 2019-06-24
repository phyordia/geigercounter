#include <HTTPClient.h>
#include <Wire.h>
#include <WiFi.h>
#include "SSD1306.h" 
#include "rnfonts.h"


#define CPM_LOG_PERIOD 15000 //Logging period in milliseconds for counts per minute
#define MINUTE_PERIOD 60000
#define BOUNCE_PERIOD 20

// Data Logger
#define LOGGER_URL "http://www.phyordia.com/geigercounter/api/logger.php"
#define LOGGER_KEY "thisismykey"

// WLAN
#define mySSID "Gojushiho"
#define myPASSWORD "Sagittarius1365"

String data_session="daq";

//Devices
SSD1306  display(0x3c, 21, 22);

//Globals


volatile unsigned long counts = 0;                       // Tube events
unsigned long cpm = 0;                                   // Counts per minute
unsigned long cpm_min = 9999999;                                   // Counts per minute
unsigned long cpm_max = 0;                                   // Counts per minute

unsigned long previousMillis;          // Counter time measurement 
unsigned long LastInterruptMillis;                       // Lst interreupt time

const int geigerPin = 4;

 
//Interrupt
// Captures count of events from Geiger counter board
void ISR_impulse() { 

  // Protect from bounce couting
  if((millis() - LastInterruptMillis) > BOUNCE_PERIOD ){
    LastInterruptMillis = millis();
    counts++;
    Serial.println("Got a hit! "+String(counts)); 
  }
}


void setup() {
  Serial.begin(115200); 
  Serial.println("Starting"); 
  displayInit();

  display.clear();
  
//   Connect to Wifi
  displayTitle("Connecting...");
  Serial.println("Connecting...");
  WiFi.begin(mySSID, myPASSWORD);
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("Connected to "+String(mySSID));
  display.clear();
  displayTitle("Connected");
  delay(2000);

  // Set up system
  pinMode(geigerPin, INPUT); 
  interrupts(); // Enable interrupts
  attachInterrupt(digitalPinToInterrupt(geigerPin), ISR_impulse, FALLING);
  
  display.clear();
  displayTitle("Measuring...");
}
 
void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis > CPM_LOG_PERIOD) {
    previousMillis = currentMillis;
    cpm = counts * MINUTE_PERIOD / CPM_LOG_PERIOD;
    if(cpm > cpm_max) cpm_max = cpm;
    if(cpm < cpm_min) cpm_min = cpm;
    
    display.clear();
    displayTitle("Radioactivity");
    displayCounts();

    // Log data
    log_data();
    
    // reset counting
    Serial.println("Resetting counters"); 
    counts = 0;   
  }
}

// Send data to server
void log_data(){
   if(WiFi.status()== WL_CONNECTED){
      writeBottomRight("^");
      HTTPClient http;   
      http.begin(LOGGER_URL);  //Specify destination for HTTP request
      http.addHeader("Content-Type", "application/json");
      http.addHeader("Authorization", LOGGER_KEY);
      String content="{\"counts\": "+String(counts)+", \"type\":\""+data_session+"\", \"log_period\":"+String(CPM_LOG_PERIOD)+"}";
      int httpResponseCode = http.POST(content);   //Send the actual POST request
       if(httpResponseCode>0){
          String response = http.getString();                       //Get the response to the request
          Serial.println(httpResponseCode);   //Print return code
          Serial.println(response);           //Print request answer
         }else{
          Serial.print("Error on sending POST: ");
          Serial.println(httpResponseCode);
         }
      http.end();  //Free resources
      display.clear();
      displayTitle("Radioactivity");
      displayCounts();
      
    }
}

// OLED Functions
void displayInit() {
  display.init();
  display.flipScreenVertically();
}

void displayTitle(String dispString) {
  display.setColor(WHITE);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(Dialog_plain_16);
  display.drawString(64, 0, dispString);
  display.display();
}

void displayCounts(){
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(Dialog_plain_24);
  display.drawString(10, 30, String(cpm));
  display.setFont(Dialog_plain_10);
  display.drawString(50, 30, "max "+String(cpm_max));
  display.drawString(50, 40, "min "+String(cpm_min));
  display.display();
}

void writeBottomRight(String s){
  Serial.println("writing "+s);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(Dialog_plain_10);
  display.drawString(100, 30, s);
  display.display();
}

