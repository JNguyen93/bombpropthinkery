//Created by Justin Nguyen
//For use by Texas Panic Room
#include <Wire.h> // Enable this line if using Arduino Uno, Mega, etc.
#include <NewTone.h>
#include <IRremote.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

int normalRate = 918;
int penaltyRate = normalRate/5;
int delayRate = normalRate;
uint16_t initial = 500;
uint16_t timer = initial;
const byte wire0 = A1;
const byte wire1 = 3;
const byte wire2 = 4;
const byte wire3 = 5;
const byte wire4 = 6;
const byte wire5 = 7;
const byte wire6 = 8;
const byte wire7 = 9;
const byte RECV_PIN = 10;
const byte buzzer = 11;
const byte signallight = 12;
const byte maglock = A0;
const byte RedLED = A2;
const byte GreenLED = A3;
const byte ppbutton = 2;
int penaltyCounter = 0;
bool flag0;
bool flag1;
bool flag2;
bool flag3;
bool flag4;
bool flag5;
bool flag6;
bool flag7;
bool correct0 = false;
bool correct1 = false;
bool correct2 = false;
bool penalty = false;
bool paused = true;
bool deductionFlag = false;
bool lose = false;
bool* flags[8];
bool escaped = false;
int flagsCap = 8;
int digit = 0;
int digitPlace = 0;
int finalTime = 0;
unsigned long lastInterrupt = 0;
bool pp;
bool ppprime = false;
Adafruit_7segment matrix = Adafruit_7segment();
IRrecv irrecv(RECV_PIN);

decode_results results;

void setup() {
#ifndef __AVR_ATtiny85__
  Serial.begin(9600);
  Serial.println("Oval Office");
#endif
  matrix.begin(0x70);
  pinMode(wire0, INPUT);
  pinMode(wire1, INPUT);
  pinMode(wire2, INPUT);
  pinMode(wire3, INPUT);
  pinMode(wire4, INPUT);
  pinMode(wire5, INPUT);
  pinMode(wire6, INPUT);
  pinMode(wire7, INPUT);
  pinMode(signallight, OUTPUT);
  pinMode(maglock, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(ppbutton, INPUT);
  pinMode(RedLED, OUTPUT);
  pinMode(GreenLED, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(ppbutton), pausePlay, RISING);
  flags[0] = &flag0;
  flags[1] = &flag1;
  flags[2] = &flag2;
  flags[3] = &flag3;
  flags[4] = &flag4;
  flags[5] = &flag5;
  flags[6] = &flag6;
  flags[7] = &flag7;
  digitalWrite(signallight, HIGH);
  digitalWrite(buzzer, LOW);
  digitalWrite(RedLED, HIGH);
  digitalWrite(GreenLED, LOW);
}

void loop() {
  Serial.print("Paused is: ");
  Serial.println(paused);
  /*if (irrecv.decode(&results)) {
    Serial.println(results.value, DEC);
    irrecv.resume(); // Receive the next value
  }

  switch(results.value) {
    case 551522415: //  Pause/Play
      paused = !paused;
      results.value = 0;
      if (!paused) {
        for(int a = 0; a < 3; a++){
          digitalWrite(signallight, LOW);
          delay(500);
          digitalWrite(signallight, HIGH);
          delay(500);
        }
      }
      else{
        digitalWrite(signallight, LOW);
        delay(1000);
        digitalWrite(signallight, HIGH);
      }
      //Serial.println("Signal Light On");
      break;
    case 551534655: //Subtract Time
      timer -= 100;
      results.value = 0;
      break;
    case 551502015: //Add Time
      timer += 100;
      results.value = 0;
      break;
    case 11815:   //Reset
      timer = initial;
      correct0 = false;
      correct1 = false;
      correct2 = false;
      paused = true;
      penalty = false;
      penaltyCounter = 0;
      deductionFlag = false;
      lose = false;
      results.value = 0;
      digitalWrite(maglock, HIGH);
      digitalWrite(signallight, HIGH);
      digitalWrite(buzzer, HIGH);
      escaped = false;
      break;
    case 551531085: //Set Time
      setTimer();
      results.value = 0;
      break;
    default:
      results.value = 0;
  }*/
  
  //wire values
  flag0 = digitalRead(wire0);
  delay(10);
  flag1 = digitalRead(wire1);
  delay(10);
  flag2 = digitalRead(wire2);
  delay(10);
  flag3 = digitalRead(wire3);
  delay(10);
  flag4 = digitalRead(wire4);
  delay(10);
  flag5 = digitalRead(wire5);
  delay(10);
  flag6 = digitalRead(wire6);
  delay(10);
  flag7 = digitalRead(wire7);
  delay(10);

  //Wire order - 4, 3, 7, 6
  penalty = false;
  for(int i = 0; i < flagsCap; i++){
    if(i <= 3){
      if(!*flags[i]){
        penalty = true;
        break;
      }
    }
    if(!*flags[i] && i == 7 && !penalty){
      correct0 = true;
    }
    if (*flags[i] && i == 7){
      correct0 = false;
    }
    if(!*flags[i] && i == 6 && !penalty && correct0){
      correct1 = true;
    }
    else if(!*flags[i] && i == 6 && !correct0){
      penalty = true;
      break;
    }
    if (*flags[i] && i == 6){
      correct1 = false;
    }
    if(!*flags[i] && i == 5 && !penalty && correct1){
      correct2 = true;
    }
    else if(!*flags[i] && i == 5 && !correct1){
      penalty = true;
      break;
    }
    if(*flags[i] && i == 5){
      correct2 = false;
    }
    if(!*flags[i] && i == 4 && !penalty && correct2){
      paused = true;
      escaped = true;
      //digitalWrite(maglock, LOW);
    }
    else if(!*flags[i] && i == 4 && !correct2){
      penalty = true;
      break;
    }
  }

  if (penalty && !paused) {
    Serial.println("PENALTY!");
    matrix.blinkRate(1);
    //matrix1.blinkRate(1);
    delayRate = penaltyRate;
    digitalWrite(buzzer, HIGH);
    digitalWrite(RedLED, LOW);
    delay(50);
    digitalWrite(RedLED, HIGH);
    /*if(!deductionFlag){
      if(penaltyCounter < 6){
        penaltyCounter++;
      }
      else{
        penaltyCounter = 6;
      }
      timer = timer - ((penaltyCounter - 1) * 100);
      deductionFlag = true;
    }*/
  }
  else {
    deductionFlag = false;
    matrix.blinkRate(0);
    //matrix1.blinkRate(0);
    delayRate = normalRate;
    digitalWrite(buzzer, LOW);
    noNewTone(buzzer);
  }
  
  Serial.print("Correct0 is ");
  Serial.println(correct0);
  Serial.print("Correct1 is ");
  Serial.println(correct1);
  Serial.print("Correct2 is ");
  Serial.println(correct2);
  Serial.print("Wire0 is ");
  Serial.println(flag0);
  Serial.print("Wire1 is ");
  Serial.println(flag1);
  Serial.print("Wire2 is ");
  Serial.println(flag2);
  Serial.print("Wire3 is ");
  Serial.println(flag3);
  Serial.print("Wire4 is ");
  Serial.println(flag4);
  Serial.print("Wire5 is ");
  Serial.println(flag5);
  Serial.print("Wire6 is ");
  Serial.println(flag6);
  Serial.print("Wire7 is ");
  Serial.println(flag7);

  if(escaped){
    digitalWrite(maglock, LOW);
    digitalWrite(GreenLED, HIGH);
    digitalWrite(RedLED, LOW);
  }
  else{
    digitalWrite(maglock, HIGH);
    digitalWrite(GreenLED, LOW);
    digitalWrite(RedLED, HIGH);
  }

  if (lose) {
    digitalWrite(signallight, LOW);
    digitalWrite(buzzer, HIGH);
  }
  
  if(!paused) { //if not paused
    
    //countdown timer
    //Serial.println(timer);
    if (timer <= 0 || timer > 10000) {
      matrix.writeDigitNum(0, 0);
      matrix.writeDigitNum(1, 0);
      matrix.writeDigitNum(3, 0);
      matrix.writeDigitNum(4, 0);
      //matrix1.writeDigitNum(0, 0);
      //matrix1.writeDigitNum(1, 0);
      //matrix1.writeDigitNum(3, 0);
      //matrix1.writeDigitNum(4, 0);
    }
    else{
      matrix.print(timer, DEC);
    }
    matrix.drawColon(true);
    matrix.writeDisplay();
    if (timer % 100 == 0) {
      timer -= 41;
    }
    else {
      timer -= 1;
    }
    if (timer <= 0 || timer > 10000) {
      paused = true;
      lose = true;
    }
    delay(delayRate);
  }
  else { // if it is paused
    if (timer <= 0 || timer > 10000) {
      matrix.writeDigitNum(0, 0);
      matrix.writeDigitNum(1, 0);
      matrix.writeDigitNum(3, 0);
      matrix.writeDigitNum(4, 0);
      //matrix1.writeDigitNum(0, 0);
      //matrix1.writeDigitNum(1, 0);
      //matrix1.writeDigitNum(3, 0);
      //matrix1.writeDigitNum(4, 0);
    }
    else {
      matrix.print(timer, DEC);
      //matrix1.print(timer, DEC);
      //noNewTone(buzzer);
    }
    penalty = false;
    matrix.drawColon(true);
    //matrix1.drawColon(true);
    matrix.blinkRate(0);
    //matrix1.blinkRate(0);
    matrix.writeDisplay();
    //matrix1.writeDisplay();
    delay(1000);
  }
}

void pausePlay(){
  if (millis() - lastInterrupt > 200){
    Serial.println("INTERRUPT!");
    if (flag0 && !flag1 && !flag2 && flag3 && !flag4 && !flag5 && !flag6 && !flag7){
        timer = initial;
        correct0 = false;
        correct1 = false;
        correct2 = false;
        paused = true;
        penalty = false;
        penaltyCounter = 0;
        deductionFlag = false;
        lose = false;
        results.value = 0;
        digitalWrite(maglock, HIGH);
        digitalWrite(signallight, HIGH);
        digitalWrite(buzzer, LOW);
        digitalWrite(RedLED, HIGH);
        digitalWrite(GreenLED, LOW);
        escaped = false;
      }
     else {
        paused = !paused;
     }
     lastInterrupt = millis(); 
  }
}

void setTimer(){
  boolean finished = false;
  matrix.print(0xFEED, HEX);
  //matrix1.print(0xFEED, HEX);
  matrix.writeDisplay();
  //matrix1.writeDisplay();
  for(int a = 0; a < 2; a++){
    digitalWrite(signallight, LOW);
    delay(500);
    digitalWrite(signallight, HIGH);
    delay(500);
  }
  results.value = 0;
  while(!finished){
    if (irrecv.decode(&results)) {
      Serial.println(results.value, DEC);
      irrecv.resume(); // Receive the next value
    }
  
    switch(results.value) {
      case 551520375:
        processTimer(1);
        results.value = 0;
        break;
      case 551504055:
        processTimer(2);
        results.value = 0;
        break;
      case 551536695:
        processTimer(3);
        results.value = 0;
        break;
      case 551495895:
        processTimer(4);
        results.value = 0;
        break;
      case 551528535:
        processTimer(5);
        results.value = 0;
        break;
      case 551512215:
        processTimer(6);
        results.value = 0;
        break;
      case 551544855:
        processTimer(7);
        results.value = 0;
        break;
      case 551491815:
        processTimer(8);
        results.value = 0;
        break;
      case 551524455:
        processTimer(9);
        results.value = 0;
        break;
      case 551487735:
        processTimer(0);
        results.value = 0;
        break;
      case 551522415:
        finished = true;
        results.value = 0;
        break;
      default:
        results.value = 0;
    }
  }
  timer = finalTime;
}

void processTimer (int x){
  if (digitPlace >= 4){
    digitPlace = 0;
    finalTime = 0;
  }
  if (digitPlace == 0){
    finalTime += (x * 1000);
    digitPlace++;
  }
  else if (digitPlace == 1){
    finalTime += (x * 100);
    digitPlace++;
  }
  else if (digitPlace == 2){
    if(x >= 6){
      x = 5;
    }
    finalTime += (x * 10);
    digitPlace++;
  }
  else if (digitPlace == 3){
    finalTime += x;
    digitPlace++;
  }
  matrix.print(finalTime, DEC);
 // matrix1.print(finalTime, DEC);
  matrix.drawColon(true);
  //matrix1.drawColon(true);
  matrix.writeDisplay();
  //matrix1.writeDisplay();
}

