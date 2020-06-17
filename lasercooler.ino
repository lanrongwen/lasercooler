const int compressorPin = 22;
const int fanPin = 24;

const int ledRedPin = 8;
const int ledBluePin = 9;
const int ledGreenPin = 10;
const int ledPowerPin = 13;

const int startButtonPin = 2;

// Will add thermestor laters


enum coolerRunStates{
  RUNCOOLER,
  STOPCOOLER,
  COOLING,
  COASTING
};


enum coolerRunStates currentState = STOPCOOLER;
int currentStartButtonState;
int lastStartButtonState;
bool switchState = false;
bool stateStarted = false;


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
      switchState = true;
    }
  }
  else{
    switchState = false;
  }
  lastStartButtonState = currentStartButtonState;

  switch(currentState){
    case RUNCOOLER:
      if (switchState == true){
        Serial.println("Will switch to STOPCOOLER");
        currentState = STOPCOOLER;
        stateStarted = false;
      }
      else if (stateStarted == false){
        Serial.println("Starting Cooler...");
        digitalWrite(fanPin, LOW);
        digitalWrite(compressorPin, LOW);
        analogWrite(ledRedPin, 0);
        analogWrite(ledBluePin, 255);
        analogWrite(ledGreenPin, 0);
        stateStarted = true;
        Serial.println("Cooler Started.");
      }
      break;
    case STOPCOOLER:
      if (switchState == true){
        Serial.println("Will switch to RUNCOOLER");
        currentState = RUNCOOLER;
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
    case COOLING:
      Serial.println("Cooling Here.");
      break;
    case COASTING:
      Serial.println("Coasting Here.");
      break;    
  }
}
