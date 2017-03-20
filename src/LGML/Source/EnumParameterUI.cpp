/*
  ==============================================================================

    EnumParameterUI.cpp
    Created: 29 Sep 2016 5:35:12pm
    Author:  bkupe

  ==============================================================================
*/

#include "EnumParameterUI.h"

EnumParameterUI::EnumParameterUI(Parameter * parameter) :
	ParameterUI(parameter),
	ep((EnumParameter *)parameter)
{
	cb.addListener(this);
	cb.setTextWhenNoChoicesAvailable(ep->niceName);
	cb.setTextWhenNothingSelected(ep->niceName);
	cb.setTooltip(ep->description);
	addAndMakeVisible(cb);

	updateComboBox();
}

EnumParameterUI::~EnumParameterUI()
{
	cb.removeListener(this);
}

void EnumParameterUI::updateComboBox()
{	
	cb.clear(dontSendNotification);


	idKeyMap.clear();
  if(DynamicObject * dob = ep->getCurrentValuesMap()){
	int id = 1;
    NamedValueSet map = dob->getProperties();
    for(auto & kv:map)
	{
    String displayed = kv.name.toString();
		cb.addItem(displayed, id);
		idKeyMap.set(id, displayed);
		keyIdMap.set(displayed, id);
		id++;
	}
  }
	cb.setSelectedId(keyIdMap[ep->getFirstSelectedId().toString()], dontSendNotification);
}

String EnumParameterUI::getCBSelectedKey()
{
	return idKeyMap[cb.getSelectedId()];
}

void EnumParameterUI::resized()
{
	cb.setBounds(getLocalBounds());
}

void EnumParameterUI::enumOptionAdded(EnumParameter *, const String &)
{
	updateComboBox();
}

void EnumParameterUI::enumOptionRemoved(EnumParameter *, const String &)
{
	updateComboBox();
}
void EnumParameterUI::enumOptionSelectionChanged(EnumParameter *,bool isSelected, const Identifier &name){
  if(keyIdMap.contains(name.toString())){
  cb.setSelectedId(keyIdMap[name.toString()],dontSendNotification);
  }
}

void EnumParameterUI::valueChanged(const var & value) 
{
  if(value.isString()){
    if(keyIdMap.contains(value)){
      cb.setSelectedId(keyIdMap[value], dontSendNotification);
      cb.setTextWhenNothingSelected(ep->niceName);
    }
    else if(value.toString()!=""){
      cb.setTextWhenNothingSelected("["+value.toString()+"]");
    }
  }
  else if (value.isObject()){
    updateComboBox();
  }
}

void EnumParameterUI::comboBoxChanged(ComboBox *)
{
	ep->setValue(getCBSelectedKey());
};
