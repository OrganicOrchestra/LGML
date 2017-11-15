#include "Arduino.h"

class LGML
{
public :
  enum IOType {DIGITAL,ANALOG};
  Stream &serial;
  LGML(Stream &s):serial(s){isIdentified = false;}
  bool isIdentified;
  void init(String description, int numInputs, int numOutputs);
  String description;
  int numInputs;
  int numOutputs;
  String * inputNames;
  String * outputNames;
  float * values;

  bool sendOnChangeOnly;

  String buffer;
  void processBuffer();
  
  void updateInputValue(int inputIndex, float value);
  
  void update();
  typedef void(*outputUpdateEvent)(String, int);
  void (*ouputUpdate) (String name, float value);
  
  
  void outputUpdate (outputUpdateEvent);
};

