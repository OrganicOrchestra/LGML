/* Copyright © Organic Orchestra, 2017
*
* This file is part of LGML.  LGML is a software to manipulate sound in realtime
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation (version 3 of the License).
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
*/


#ifndef TRIGGER_H_INCLUDED
#define TRIGGER_H_INCLUDED

#include "Parameter.h"
#include "QueuedNotifier.h"

class Trigger : public Parameter
{
public:
	Trigger(const String &niceName, const String &description, bool enabled = true);
	~Trigger() {masterReference.clear();}
  
	
	
	
	
	
	
  DynamicObject * createDynamicObject() override;
  var getVarObject() override;
  var getVarState() override;
  void setFromVarObject(DynamicObject & ob) override;

  static const var triggerVar;
	void trigger()
	{
    setValue(triggerVar);
	}
  void tryToSetValue(var _value, bool silentSet , bool force) override{
    if(!waitOrDeffer(_value, silentSet, force)){
      isSettingValue = true;
      if (!silentSet) notifyValueChanged();
      isSettingValue = false;
    }
    
  }

public:


	
private:
	WeakReference<Trigger>::Master masterReference;
	friend class WeakReference<Trigger>;
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Trigger)
};



#endif  // TRIGGER_H_INCLUDED
