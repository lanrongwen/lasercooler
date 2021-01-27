#include <DS18B20.h>
#include <elapsedMillis.h>
#include <SoftwareSerial.h>

// Input Pins
const int START_STOP_BUTTON_PIN = 50;
const int TEMP_SENSOR_PIN = 52;


// Output Pins
const int COMPRESSOR_PIN = 8;
const int FAN_PIN = 9;

const int LED_RED_PIN = 2;
const int LED_GREEN_PIN = 3;
const int LED_BLUE_PIN = 4;

const int LCD_PIN = 5;
const int LCD_UNUSED_PIN = 6;

//TempSensor Info
uint8_t compressorSensor[] = {40, 26, 187, 7, 214, 1, 60, 34};
uint8_t reservoirSensor[] = {40, 161, 144, 7, 214, 1, 60, 85};
float compressorTemp = 0;
float resevoirTemp = 0;

//colors
uint8_t white[] = {255, 255, 255};  
uint8_t red[] = {255, 0, 0};  
uint8_t green[] = {0, 255, 0};  
uint8_t blue[] = {0, 0, 255};  
uint8_t lightRed[] = {122, 0, 0};  
uint8_t lightGreen[] = {0, 122, 0};  
uint8_t lightBlue[] = {0, 0, 122};  

// LCD Setup
SoftwareSerial lcd = SoftwareSerial(LCD_UNUSED_PIN, LCD_PIN);
unsigned long lcdRefreshTime = 0;
String tempString;
String displayMessage;

// Temp Sensor Setup
DS18B20 tempSensors(TEMP_SENSOR_PIN);

enum coolerRunStates{
  RUNCOOLER,
  RESTCOOLER,
  STOPCOOLER,
  COASTING
};


enum coolerRunStates currentState = STOPCOOLER;
int currentStartButtonState;
int lastStartButtonState;
bool buttonHanled = true;
bool stateStarted = false;

//Setup Timers And Triggers
elapsedMillis timeElapsed;
const unsigned long COOLERMAXRUNTIME = 900000; // (15 * 60 * 1000 - 15 minutes)
const unsigned long COOLERRESTTIME = 240000; // (4 * 60 * 1000 -4 minutes)
const float COOLERHIGHTEMP = 67;
const float COOLERLOWTEMP = 60;

void setup() {
  if(!Serial.available()){
    Serial.begin(9600);
  }

  Serial.println("Setting up LCD");
  lcd.begin(9600);

  lcdClearScreen();
  lcdSetColor(lightRed);

  lcd.println("Laser Cooler");
  lcd.print("Booting Up...");
  
  Serial.println("Setting Up Relays");
  pinMode(COMPRESSOR_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  digitalWrite(COMPRESSOR_PIN, HIGH);
  digitalWrite(FAN_PIN, HIGH);

  Serial.println("Setting Up LEDs");
  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(LED_BLUE_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);
  ledSetColor(white);

  Serial.println("Setting Up Buttons");
  pinMode(START_STOP_BUTTON_PIN, INPUT_PULLUP);
  currentStartButtonState = digitalRead(START_STOP_BUTTON_PIN);
  lastStartButtonState = currentStartButtonState;

  Serial.println("Setting up Misc...");
  tempString = String();
  readTempSensors();
}



void lcdClearScreen(){
  // clear screen
  lcd.write(0xFE);
  lcd.write(0x58);
  delay(10);   // we suggest putting delays after each command 
}


void lcdSetColor(uint8_t color[]){
  lcd.write(0xFE);
  lcd.write(0xD0);
  lcd.write(color[0]); 
  lcd.write(color[1]);
  lcd.write(color[2]);
  delay(10);
}



void ledSetColor(uint8_t color[]){
  analogWrite(LED_RED_PIN, color[0]);
  analogWrite(LED_GREEN_PIN, color[1]);
  analogWrite(LED_BLUE_PIN, color[2]);
}



void readTempSensors(){
  tempSensors.select(reservoirSensor);
  resevoirTemp = tempSensors.getTempF();
  tempSensors.select(compressorSensor);
  compressorTemp = tempSensors.getTempF();
}


void readButton(){
  currentStartButtonState = digitalRead(START_STOP_BUTTON_PIN);
  if (currentStartButtonState != lastStartButtonState){
    delay(50);
    if (currentStartButtonState != lastStartButtonState){
      int currentStartButtonState = digitalRead(START_STOP_BUTTON_PIN);
      lastStartButtonState = currentStartButtonState;
      if (currentStartButtonState == LOW){
        Serial.println("Button has been pushed.");
        buttonHanled = false;
      }
      else{
        Serial.println("Button has been released.");
      }
    }
  }
}


void refreshDisplays(){
  lcdClearScreen(); 
    lcd.println(tempString + "R:" + resevoirTemp + " C:" + compressorTemp );
    lcd.print(displayMessage);
    lcdRefreshTime = millis();
}





void loop() {
  readButton();
  // Serial.println(lastStartButtonState);
  

  if (millis() - lcdRefreshTime > 2000){
    readTempSensors();
    refreshDisplays();
  }

  switch(currentState){
    case RUNCOOLER:
      if (buttonHanled == false){
        Serial.println("Will switch to STOPCOOLER");
        currentState = STOPCOOLER;
        stateStarted = false;
        buttonHanled = true;
      }
      else if (stateStarted == false){
        Serial.println("Starting Active Cooler...");
        digitalWrite(FAN_PIN, LOW);
        digitalWrite(COMPRESSOR_PIN, LOW);
        lcdSetColor(blue);
        ledSetColor(blue);
        stateStarted = true;
        displayMessage = "Cooler Active!";
        refreshDisplays();
        timeElapsed = 0;
        Serial.println("Cooler Started.");
      }
      else{
        if (resevoirTemp < COOLERLOWTEMP){
          Serial.println("Cooler has hit is cool point, will now let it coast...");
          currentState = COASTING;
          stateStarted = false;
        }
        else if (timeElapsed > COOLERMAXRUNTIME){
          Serial.println("Hit maximum active cooler time limit, will go to RESTCOOLER.");
          currentState = RESTCOOLER;
          stateStarted = false;
        }
      }
      break;
    case RESTCOOLER:
      if (buttonHanled == false){
        Serial.println("Will switch to STOPCOOLER");
        currentState = STOPCOOLER;
        stateStarted = false;
        buttonHanled = true;
      }
      else if (stateStarted == false){
        Serial.println("Starting Cooler...");
        digitalWrite(FAN_PIN, LOW);
        digitalWrite(COMPRESSOR_PIN, HIGH);
        lcdSetColor(lightBlue);
        ledSetColor(lightBlue);
        stateStarted = true;
        displayMessage = "Cooler Resting.";
        refreshDisplays();
        timeElapsed = 0;
        Serial.println("Cooler Started.");
      }
      else{
        if (timeElapsed > COOLERRESTTIME){
          Serial.println("Hit maximum rest cooler time limit, will go to RUNCOOLER.");
          currentState = RUNCOOLER;
          stateStarted = false;
        }
      }
      break;
    case STOPCOOLER:
      if (buttonHanled == false){
        Serial.println("Will switch to RUNCOOLER");
        currentState = RUNCOOLER;
        stateStarted = false;
        buttonHanled = true;
      }
      else if (stateStarted == false){
        Serial.println("Stopping Cooler...");
        digitalWrite(FAN_PIN, HIGH);
        digitalWrite(COMPRESSOR_PIN, HIGH);
        lcdSetColor(white);
        ledSetColor(white);
        stateStarted = true;
        displayMessage = "Cooler Stopped.";
        refreshDisplays();
        Serial.println("Cooler Stopped.");
      }
      break;
    case COASTING:
      if (buttonHanled == false){
        Serial.println("Will switch to STOPCOOLER");
        currentState = STOPCOOLER;
        stateStarted = false;
        buttonHanled = true;
      }
      else if (stateStarted == false){
        Serial.println("Setting Cooler to Coast Mode.");
        digitalWrite(FAN_PIN, HIGH);
        digitalWrite(COMPRESSOR_PIN, HIGH);
        lcdSetColor(green);
        ledSetColor(green);
        stateStarted = true;
        displayMessage = "Cooler Coast";
        refreshDisplays();
        Serial.println("Cooler Is in Coast.");
      }
      else{
        if (resevoirTemp > COOLERHIGHTEMP){
          Serial.println("Cooler has hit is hot point, will now turn on the cooler...");
          currentState = RUNCOOLER;
          stateStarted = false;
        }
      }
      break;
  }
}
