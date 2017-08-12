#include "LGML.h"

LGML lgml(Serial);

#define NUM_ANALOG_INPUTS   6
#define NUM_DIGITAL_INPUTS  11
#define NUM_INPUTS          NUM_ANALOG_INPUTS + NUM_DIGITAL_INPUTS

int analogPins[NUM_ANALOG_INPUTS] = {A0,A1,A2,A3,A4,A5};
int digitalPins[NUM_DIGITAL_INPUTS] = {2,3,4,5,6,7,8,9,10,11,12}; //keep pin 13 for led

String pinsNames[NUM_INPUTS];
void setup() {

  Serial.begin(9600);

 //Serial.println("SETUP");
  for(int i=0;i<NUM_ANALOG_INPUTS;i++) pinsNames[i] = String("A")+String(i);
  for(int i=0;i<NUM_DIGITAL_INPUTS;i++) pinsNames[i+NUM_ANALOG_INPUTS] = String("D")+String(i+2);

  lgml.init("Example D & A Inputs", NUM_ANALOG_INPUTS + NUM_DIGITAL_INPUTS, 0);
  lgml.inputNames = pinsNames;
  
  pinMode(13, OUTPUT);

  for(int i=0;i<NUM_ANALOG_INPUTS;i++) pinMode(analogPins[i],INPUT);
  for(int i=0;i<NUM_DIGITAL_INPUTS;i++) pinMode(digitalPins[i],INPUT_PULLUP);
  
  digitalWrite(13,HIGH);

  //Serial.println("SETUP FINISH");
}

void loop() {
  
  //Serial.println("LOOP");
  lgml.update();

 
  for(int i=0;i<NUM_ANALOG_INPUTS;i++)
  {
    float val = analogRead(analogPins[i])*1.00/1024.000;
    lgml.updateInputValue(i, val);
  }

   for(int i=0;i<NUM_DIGITAL_INPUTS;i++)
  {
    float val = digitalRead(digitalPins[i])?1:0;
    lgml.updateInputValue(i+NUM_ANALOG_INPUTS, val);
  }
  

  delay(20);
}
