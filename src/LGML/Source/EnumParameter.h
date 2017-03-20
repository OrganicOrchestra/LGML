/*
  ==============================================================================

    EnumParameter.h
    Created: 29 Sep 2016 5:34:59pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef ENUMPARAMETER_H_INCLUDED
#define ENUMPARAMETER_H_INCLUDED

#include "Parameter.h"

class EnumParameterUI;

class EnumParameter : public Parameter
{
public:
	EnumParameter(const String &niceName, const String &description, bool enabled = true);
  ~EnumParameter() ;

	void addOption(Identifier key, var data);
	void removeOption(Identifier key);
  void selectId(Identifier key,bool shouldSelect,bool appendSelection = true);



  
  void setValueInternal(var & _value) override;

  Array<Identifier> getSelectedIds() ;
  Identifier getFirstSelectedId() ;





  Array<var> getSelectedValues();
  var getFirstSelectedValue(var defaultValue=var::null) ;
  
  var getValueForId(const Identifier &i);
	EnumParameterUI * createUI(EnumParameter * target = nullptr);
	ControllableUI * createDefaultUI(Controllable * targetControllable = nullptr) override;


	//Listener
	class  Listener
	{
	public:
		/** Destructor. */
		virtual ~Listener() {}
		virtual void enumOptionAdded(EnumParameter *, const String &) = 0;
		virtual void enumOptionRemoved(EnumParameter *, const String &) = 0;
    virtual void enumOptionSelectionChanged(EnumParameter *,bool isSelected, const Identifier &){};
	};

	ListenerList<Listener> enumListeners;
	void addEnumParameterListener(Listener* newListener) { enumListeners.add(newListener); }
	void removeEnumParameterListener(Listener* listener) { enumListeners.remove(listener); }

  DynamicObject * getCurrentValuesMap();
  

private:
  DynamicObject * getValuesMap(const var & v);
  Array<Identifier> getSelectedSetIds(const var & v);
  Array<var> *getSelectedSet(const var & v);

  DynamicObject * enumData;
  static Identifier valueMapIdentifier;
  static Identifier selectedSetIdentifier;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EnumParameter)
};


#endif  // ENUMPARAMETER_H_INCLUDED
