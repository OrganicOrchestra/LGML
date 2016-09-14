#include "LGML.h"

void LGML::init(String description, int numInputs, int numOutputs)
{

  this->sendOnChangeOnly = true;
  this->description = description;
  
  this->numInputs = numInputs;
  this->numOutputs = numOutputs;
  inputNames = new String[0];
  outputNames = new String[0];
  values = new float[numInputs];
  buffer = "";
}

void LGML::update()
{
  
  while(serial.available() > 0)
  {
    char c = serial.read();
    switch(c)
    {
      case '\r': //nothing
      break;
      
      case '\n':
      processBuffer();
      buffer = "";
      break;

      default:
      buffer += c;
      break;
    }
  }
}

void LGML::processBuffer()
{
  char command = buffer[0];
  switch(command)
  {
    case 'i':
      serial.print("i "); //i for identifications
      serial.print(description);
      serial.print("\n");

      for(int i=0;i<numInputs;i++)
      {
        serial.print("a "); //a for inputs
        serial.print(inputNames[i]);
        serial.print("\n");
      }
    
      for(int i=0;i<numOutputs;i++)
      {
        serial.print("o "); //o for inputs
        serial.print(outputNames[i]);
        serial.print("\n");
      }

      isIdentified = true;
      digitalWrite(13,HIGH);
      break;
  }
}

void LGML::updateInputValue(int inputIndex, float value)
{
  if(!isIdentified) return;

  if(sendOnChangeOnly && values[inputIndex] == value) return;
  
  serial.print("u "); //update
  serial.print(inputNames[inputIndex]);
  serial.print(" ");
  serial.print(value);
  serial.print("\n");

  values[inputIndex] = value;
}
  
