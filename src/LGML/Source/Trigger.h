/*
 ==============================================================================
 
 Trigger.h
 Created: 8 Mar 2016 1:09:29pm
 Author:  bkupe
 
 ==============================================================================
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
  void tryToSetValue(var _value, bool silentSet , bool force ,bool defferIt) override{
    jassert(isSettingValue==false);
    isSettingValue = true;
    if (!silentSet) notifyValueChanged(defferIt);
    isSettingValue = false;
    
  }

public:


	
private:
	WeakReference<Trigger>::Master masterReference;
	friend class WeakReference<Trigger>;
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Trigger)
};



#endif  // TRIGGER_H_INCLUDED
