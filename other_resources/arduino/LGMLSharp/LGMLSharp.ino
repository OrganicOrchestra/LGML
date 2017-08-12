#include "SharpIR.h"
#include "LGML.h"

SharpIR sharp(A0,1080);

LGML lgml(Serial);
#define NUM_INPUTS  1
int pins[NUM_INPUTS] = {A0}; //inclure toutes les entrées qu'on veut
String pinsNames[NUM_INPUTS] = {"Sharp"};

void setup() {

  Serial.begin(9600);

 //Initialisation LGML
  lgml.init("BAM Sharp", NUM_INPUTS,0);
  lgml.inputNames = pinsNames;
  
}

void loop() {
  float value = sharp.distance()*1.;
  float mappedValue = (value-7)/30.;

  lgml.updateInputValue(0,mappedValue);  
  
  lgml.update();
  delay(10);
}
