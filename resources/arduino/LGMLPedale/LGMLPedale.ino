#include "LGML.h"

LGML lgml(Serial);

#define NUM_DIGITAL_INPUTS  2
#define NUM_INPUTS         NUM_DIGITAL_INPUTS

int digitalPins[NUM_DIGITAL_INPUTS] = {2,3};//,4,5,6,7,8,9,10,11,12}; //keep pin 13 for led

String pinsNames[NUM_INPUTS];
void setup() {

  Serial.begin(9600);

  for(int i=0;i<NUM_DIGITAL_INPUTS;i++) pinsNames[i] = String("D")+String(i+2);

  lgml.init("LaPetiteMechanteLoop", NUM_DIGITAL_INPUTS, 0);
  lgml.inputNames = pinsNames;
  
  pinMode(13, OUTPUT);

  for(int i=0;i<NUM_DIGITAL_INPUTS;i++) pinMode(digitalPins[i],INPUT_PULLUP);
  
  digitalWrite(13,HIGH);

  //Serial.println("SETUP FINISH");
}

void loop() {
  
  //Serial.println("LOOP");
  lgml.update();
   for(int i=0;i<NUM_DIGITAL_INPUTS;i++)
  {
    float val = !digitalRead(digitalPins[i])?1:0;
    if(i == 0) val = !val; //Une des 2 pedales a un bouton qui fonctionne à l'inverse
   
    if(lgml.values[i] != val)
    {
      lgml.updateInputValue(i, val);
    }
  }
  

  delay(20);
}
