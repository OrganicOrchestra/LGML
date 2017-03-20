/*
 ==============================================================================

 EnumParameter.cpp
 Created: 29 Sep 2016 5:34:59pm
 Author:  bkupe

 ==============================================================================
 */

#include "EnumParameter.h"
#include "EnumParameterUI.h"

Identifier EnumParameter::selectedSetIdentifier("selected");
Identifier EnumParameter::valueMapIdentifier("values");

EnumParameter::EnumParameter(const String & niceName, const String &description, bool enabled) :
Parameter(Type::ENUM, niceName, description,var(),var(),var(), enabled)
{
  enumData = new DynamicObject();
  enumData->setProperty(valueMapIdentifier, new DynamicObject());
  enumData->setProperty(selectedSetIdentifier, Array<var>());
  value = var(enumData);

}
EnumParameter::~EnumParameter(){
//  value = var::null;
  enumData=nullptr;
}

void EnumParameter::addOption(Identifier key, var data)
{
  auto vm = getValuesMap(value);
  jassert(!vm->hasProperty(key));
  vm->setProperty(key, data);
  enumListeners.call(&Listener::enumOptionAdded, this, key.toString());
}

void EnumParameter::removeOption(Identifier key)
{
  enumListeners.call(&Listener::enumOptionRemoved, this, key.toString());
  getValuesMap(value)->removeProperty(key);

}
Array<Identifier> EnumParameter::getSelectedIds() {
  return getSelectedSetIds(value);
}

Identifier EnumParameter::getFirstSelectedId() {
  Array<Identifier> arr = getSelectedIds();
  if(arr.size())return arr[0];
  return Identifier::null;

}
var EnumParameter::getFirstSelectedValue(var defaultValue) {
  Array<var> arr = getSelectedValues();
  if(arr.size())return arr[0];
  return defaultValue;

}


DynamicObject * EnumParameter::getCurrentValuesMap(){
  return getValuesMap(value);
}



void EnumParameter::selectId(Identifier key,bool shouldSelect,bool appendSelection){
  if(!appendSelection){
    for (auto & s:getSelectedSetIds(value)){
      selectId(s, false,true);
    }
  }
  Array<var> * selection = getSelectedSet(value);
  jassert(selection);
  if(!appendSelection)selection->clear();
  if(shouldSelect){
    selection->add(key.toString());
  }
  else{
    selection->removeAllInstancesOf(key.toString());

  }
  enumListeners.call(&Listener::enumOptionSelectionChanged, this,shouldSelect, key.toString());


}




Array<var> EnumParameter::getSelectedValues(){
  Array<var> res;
  DynamicObject * vm = getValuesMap(value);
  for(auto & i: getSelectedSetIds(value)){
    res.add( vm->getProperty(i));
  }
  return res;
};
var EnumParameter::getValueForId(const Identifier &i){
  if(DynamicObject * dob = getCurrentValuesMap()){
    return dob->getProperty(i);

  }
  return var::null;
}




void EnumParameter::setValueInternal(var & _value){
  if (_value.isInt()){
    Identifier key = getCurrentValuesMap()->getProperties().getName((int)_value);
    selectId(key,true,false);

  }
  else if(_value.isString()){
    selectId(_value.toString(),true,false);
  }
  
  else if(DynamicObject * values = getValuesMap(_value)){

    if(DynamicObject * oldValues = getValuesMap(value)){
      for(auto v:oldValues->getProperties()){
        removeOption(v.name);
      }
    }
    for(auto v:values->getProperties()){
      addOption(v.name.toString(), v.value);
    }


    for(auto sel:getSelectedSetIds(value)){
      selectId(sel.toString(),false,true);
    }



    for (auto & sel:getSelectedSetIds(_value)){
      selectId(sel.toString(),true,true);
    }




  }
  else{
    // var not suported
    jassertfalse;

  }


}


DynamicObject * EnumParameter::getValuesMap(const var & v){

  if(DynamicObject *dob = v.getDynamicObject()){
    return dob->getProperty(valueMapIdentifier).getDynamicObject();
  }
  jassertfalse;
  return nullptr;

}

Array<Identifier> EnumParameter::getSelectedSetIds(const juce::var &v){
  Array<Identifier>res;
  if(Array<var> *  arr =  getSelectedSet(v)){
    for(auto &e:*arr){
      res.add(Identifier(e.toString()));
    }
  }

  return res;
}

Array<var> * EnumParameter::getSelectedSet(const juce::var &v){
  if(DynamicObject *dob = v.getDynamicObject()){
    return dob->getProperty(selectedSetIdentifier).getArray();
  }
  return nullptr;
}




////////////
//ui
EnumParameterUI * EnumParameter::createUI(EnumParameter * target)
{
  if (target == nullptr) target = this;
  return new EnumParameterUI(target);
}

ControllableUI * EnumParameter::createDefaultUI(Controllable * targetControllable) {
  return createUI(dynamic_cast<EnumParameter *>(targetControllable));
}
