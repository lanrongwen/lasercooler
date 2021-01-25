const int compressorPin = 22;
const int fanPin = 24;

const int ledRedPin = 8;
const int ledBluePin = 9;
const int ledGreenPin = 10;
const int ledPowerPin = 13;

const int startButtonPin = 2;

// Will add thermestor laters


enum coolerRunStates{
  RUNCOOLERACTIVE,
  RESTCOOLER,
  STOPCOOLER,
  COASTING
};


enum coolerRunStates currentState = STOPCOOLER;
int currentStartButtonState;
int lastStartButtonState;
bool userSwitchState = false;
bool stateStarted = false;
unsigned long stateStartTime = 0;
unsigned long currentTime = 0;
unsigned long currentStateEndTime = 0;
const unsigned long ACTIVECOOLERTIMEOUT = 15 * 60 * 1000; //(15 minutes)
const unsigned long RESTCOOLERTIMEOUT = 4 * 60 * 1000; //(4 minutes)

void setup() {
  if(!Serial.available()){
    Serial.begin(9600);
  }
  
  Serial.println("Setting Up Relays");
  pinMode(compressorPin, OUTPUT);
  pinMode(fanPin, OUTPUT);
  digitalWrite(compressorPin, HIGH);
  digitalWrite(fanPin, HIGH);

  Serial.println("Setting Up LEDs");
  pinMode(ledRedPin, OUTPUT);
  pinMode(ledBluePin, OUTPUT);
  pinMode(ledGreenPin, OUTPUT);
  pinMode(ledPowerPin, OUTPUT);
  analogWrite(ledRedPin, 255);
  analogWrite(ledBluePin, 255);
  analogWrite(ledGreenPin, 255);
  digitalWrite(ledPowerPin, HIGH);

  Serial.println("Setting Up Buttons");
  pinMode(startButtonPin, INPUT_PULLUP);
  currentStartButtonState = digitalRead(startButtonPin);
  lastStartButtonState = currentStartButtonState;
}

void loop() {
  int currentStartButtonState = digitalRead(startButtonPin);
  if (currentStartButtonState == LOW && lastStartButtonState == HIGH){
    delay(50);
    int currentStartButtonState = digitalRead(startButtonPin);
    if (currentStartButtonState == LOW){
      Serial.println("Will Change State");
      lastStartButtonState = currentStartButtonState;
      userSwitchState = true;
    }
  }
  else{
    userSwitchState = false;
  }
  lastStartButtonState = currentStartButtonState;

  switch(currentState){
    case RUNCOOLERACTIVE:
      if (userSwitchState == true){
        Serial.println("Will switch to STOPCOOLER");
        currentState = STOPCOOLER;
        stateStarted = false;
      }
      else if (stateStarted == false){
        Serial.println("Starting Active Cooler...");
        stateStartTime = millis();
        currentStateEndTime = stateStartTime + ACTIVECOOLERTIMEOUT;
        digitalWrite(fanPin, LOW);
        digitalWrite(compressorPin, LOW);
        analogWrite(ledRedPin, 0);
        analogWrite(ledBluePin, 255);
        analogWrite(ledGreenPin, 0);
        stateStarted = true;
        Serial.println("Cooler Started.");
      }
      else{
        currentTime = millis();
        if (currentTime < currentStateEndTime){
          Serial.println("Hit maximum active cooler time limit, will go to RESTCOOLER.")
          currentState = RESTCOOLER;
          stateStarted = false;
        }
      }
      break;
    case RESTCOOLER:
      if (userSwitchState == true){
        Serial.println("Will switch to STOPCOOLER");
        currentState = STOPCOOLER;
        stateStarted = false;
      }
      else if (stateStarted == false){
        Serial.println("Starting Cooler...");
        stateStartTime = millis();
        currentStateEndTime = stateStartTime + RESTCOOLERTIMEOUT;
        digitalWrite(fanPin, LOW);
        digitalWrite(compressorPin, HIGH);
        analogWrite(ledRedPin, 0);
        analogWrite(ledBluePin, 125);
        analogWrite(ledGreenPin, 0);
        stateStarted = true;
        Serial.println("Cooler Started.");
      }
      else{
        currentTime = millis();
        if (currentTime < currentStateEndTime){
          Serial.println("Hit maximum rest cooler time limit, will go to RUNCOOLERACTIVE.")
          currentState = RUNCOOLERACTIVE;
          stateStarted = false;
        }
      }
      break;
    case STOPCOOLER:
      if (userSwitchState == true){
        Serial.println("Will switch to RUNCOOLERACTIVE");
        currentState = RUNCOOLERACTIVE;
        stateStarted = false;
      }
      else if (stateStarted == false){
        Serial.println("Stopping Cooler...");
        digitalWrite(fanPin, HIGH);
        digitalWrite(compressorPin, HIGH);
        analogWrite(ledRedPin, 255);
        analogWrite(ledBluePin, 0);
        analogWrite(ledGreenPin, 0);
        stateStarted = true;
        Serial.println("Cooler Stopped.");
      }
      break;
    case COASTING:
      Serial.println("Coasting Here.");
      break;    
  }
}
