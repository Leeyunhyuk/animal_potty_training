#include <Servo.h>

Servo cap, pan, tilt;

const int max_pos =  73;
const int min_pos =  43;
const int pos_stp =  30;

int pos = 43;
int flag = 1;

const int pinTrig1 = 4;
const int pinEcho1 = 5;

const int pinTrig2 = 6;
const int pinEcho2 = 7;

char incoming_byte;
char sm;

void setup() {
    cap.attach(9);  
    pan.attach(10);
    tilt.attach(11);  
    
    pinMode(pinTrig1, OUTPUT);
    pinMode(pinEcho1, INPUT );
    
    pinMode(pinTrig2, OUTPUT);
    pinMode(pinEcho2, INPUT );
    
    Serial.begin(115200);
    
    pan.write(88);
    tilt.write(170);
  }
void loop() {
    char incoming_byte = Serial.read();  
    while(flag == 1) {    
      trig(pinTrig1); 
      unsigned long echo_time1 = get_echoTime(pinEcho1);  
      trig(pinTrig2);
      unsigned long echo_time2 = get_echoTime(pinEcho2);  
        
      if(echo_time1!= 0 && echo_time2 != 0){
        sm = '1';
        delay(30);
        Serial.write(sm);
      }
      
      SerialEvent();
    }
    if (incoming_byte == 'a') {
      pos = max_pos;
      cap.write(pos);
      delay(100);
      pos = min_pos;
      cap.write(pos);
      flag = 1;
    }
}

void trig(const int trig) {
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW );
}

unsigned long get_echoTime(const int echo) {
  
  unsigned long echoTime = 0;
  
  while(digitalRead(echo) == LOW );
  
  unsigned long startTime = micros();
  
  while(digitalRead(echo) == HIGH);
  
  echoTime = micros() - startTime;
  
    if(echoTime >= 240 && echoTime <= 23000) {
      return echoTime;
    } else {
    sm = '0'; 
    Serial.write(sm);
    }
}

void SerialEvent(){
  incoming_byte = Serial.read();
  
  if (incoming_byte == 'p'){
   flag = 0;
  }
   delay(20);
}
