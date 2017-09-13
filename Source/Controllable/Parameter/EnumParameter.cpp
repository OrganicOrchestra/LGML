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


#include "EnumParameter.h"

#include "ParameterFactory.h"
REGISTER_PARAM_TYPE(EnumParameter)

Identifier EnumParameter::modelIdentifier("model");
Identifier EnumParameter::selectedSetIdentifier("selected");

///////////////////
// EnumParameter

EnumParameter::EnumParameter(const String & niceName, const String &description, EnumParameterModel * modelInstance,bool _userCanEnterText, bool enabled) :
Parameter( niceName, description,var::undefined(), enabled),
asyncNotifier(1000),
userCanEnterText(_userCanEnterText)
{
  enumData = new DynamicObject();
  enumData->setProperty(selectedSetIdentifier, Array<var>());
  ownModel = modelInstance==nullptr;
  model = ownModel?new EnumParameterModel(true):modelInstance;
  enumData->setProperty(modelIdentifier, model.get()->getObject() );


  model->listeners.add(this);

  asyncNotifier.addListener(this);

  value = var(enumData);

}
EnumParameter::~EnumParameter(){
  //  value = var::null;
  model->listeners.remove(this);
  asyncNotifier.removeListener(this);
  enumData=nullptr;
}

EnumParameterModel * EnumParameter::getModel() const{
  jassert(model.get());
  return model.get();

}

void EnumParameter::addOption(Identifier key, var data,bool user)
{
  //  adding option thru parameter is not supported when using a shared model
  jassert(user || ownModel || userCanEnterText);
  getModel()->addOption(key,data,user);
}

void EnumParameter::addOrSetOption(Identifier key, var data,bool user)
{
  //  adding option thru parameter is not supported when using a shared model
  jassert(ownModel|| userCanEnterText);
  getModel()->addOrSetOption(key,data);
}
void EnumParameter::removeOption(Identifier key)
{
  //  removing option thru parameter is not supported when using a shared model
  jassert(ownModel || userCanEnterText);
  selectId(key, false,true);
  getModel()->removeOption(key);



}
Array<Identifier> EnumParameter::getSelectedIds() const{
  return getSelectedSetIds(value);
}

Identifier EnumParameter::getFirstSelectedId() const{
  Array<Identifier> arr = getSelectedIds();
  if(arr.size())return arr[0];
  return Identifier::null;

}
var EnumParameter::getFirstSelectedValue(var _defaultValue) const{
  Array<var> arr = getSelectedValues();
  if(arr.size())return arr[0];
  return _defaultValue;

}


bool EnumParameter::selectionIsNotEmpty(){
  Array<var> arr = getSelectedValues();
  return arr.size()>0;
}


void EnumParameter::selectId(Identifier key,bool shouldSelect,bool appendSelection){
  if(!appendSelection){
    auto oldS = getSelectedIds();
      for (auto & s:oldS){
        if(s!=key){
          selectId(s, false,true);
        }
      }

  }
  Array<var> * selection = getSelectedSet(value);
  jassert(selection);
  jassert(appendSelection || (selection && (selection->size()==0 || selection->getReference(0).toString()==key.toString())));
  //  if(!appendSelection)selection->clear();
  int numSelectionChange = 0;
  if(shouldSelect  && !selection->contains(key.toString())){
    selection->add(key.toString());
    numSelectionChange = 1;
  }
  else if(!shouldSelect){
    numSelectionChange = selection->removeAllInstancesOf(key.toString());

  }
  if(numSelectionChange>0){
    auto msg = EnumChangeMessage::newSelectionMessage(key, shouldSelect, getModel()->isValidId(key));
    processForMessage(*msg, enumListeners);
    asyncNotifier.addMessage(msg);
    DBG("enum : " << key.toString() << (!shouldSelect?" not":"")<<" selected"<<(!msg->isValid?" not":"")<<" valid ");


  }


}

bool EnumParameter::selectFromVar(var & _value,bool shouldSelect,bool appendSelection){
  // select based on integer
  if (_value.isInt()){
    const int idx = (int)_value;
    auto props = getModel()->getProperties();

     if(idx>=0 && idx<props.size()){
      Identifier key = props.getName(idx);
      selectId(key,shouldSelect,appendSelection);
    }
    else{
      unselectAll();
    }
    return true;

  }

  // select based on string
  else if(_value.isString()){
    String sV = _value.toString();
    if(sV.isEmpty()){
      unselectAll();
    }
    else{
      selectId(sV,shouldSelect,appendSelection);
    }
    return true;
  }
  else {
    return false;
  }
}


void EnumParameter::unselectAll(){
  for (auto & s:getSelectedSetIds(value)){
    selectId(s, false,true);
  }
  jassert(getSelectedSet(value)->size()==0) ;
}




Array<var> EnumParameter::getSelectedValues() const {
  Array<var> res;
  auto vm = getModel()->getProperties();
  for(auto & i: getSelectedSetIds(value)){
    var * s =  vm.getVarPointer(i);
    if(s) {res.add(*s );}
    else  {jassertfalse;}
  }
  return res;
};

var EnumParameter::getValueForId(const Identifier &i){
  return getModel()->getValueForId(i);
}



void EnumParameter::setValueInternal(var & _value){
  // try to select
  DBG("enum value : " <<_value.toString());
  if(selectFromVar(_value,true,false)){}
  else if(_value.isUndefined()){
    unselectAll();
  }
  else if(_value.isArray()){
    unselectAll();
    for(auto &v : *_value.getArray()){
      jassert(selectFromVar(v,true,true));
    }
  }

  // rebuild the whole model if needed and select
  else if(DynamicObject * dvalues = _value.getProperty(modelIdentifier,var::null).getDynamicObject()){

    // if model is stored, this param should own it

    if(auto model = getModel()){
        auto oldP = model->getProperties();
        for(auto v:oldP){
          removeOption(v.name);
        }

      }
      for(auto v:dvalues->getProperties()){
        addOption(v.name.toString(), v.value);
      }



    for(auto sel:getSelectedSetIds(value)){
      selectId(sel.toString(),false,true);
    }

    for (auto & sel:getSelectedSetIds(_value)){
      selectId(sel.toString(),true,true);
    }
  }
  else if(Array<var>* vl = getSelectedSet(_value)){
    for (auto s:*vl){selectId(s.toString(),true,true);}
    if(vl->size()==0){unselectAll();}
  }
  else{
    // var not suported
    jassertfalse;

  }


}




Array<Identifier> EnumParameter::getSelectedSetIds(const juce::var &v) const{
  Array<Identifier>res;
  if(Array<var> *  arr =  getSelectedSet(v)){
    for(auto &e:*arr){
      res.add(Identifier(e.toString()));
    }
  }

  return res;
}

Array<var> * EnumParameter::getSelectedSet(const juce::var &v) const{
  if(DynamicObject *dob = v.getDynamicObject()){
    return dob->getProperty(selectedSetIdentifier).getArray();
  }
  // wrong var passed in
//  jassertfalse;
  return nullptr;
}


void EnumParameter::modelOptionAdded(EnumParameterModel *,Identifier & key ) {
  auto msg = EnumChangeMessage::newStructureChangeMessage(key, true);
  if(getSelectedIds().contains(key)){
    msg->isSelectionChange = true;
    msg->isSelected = true;
    Parameter::notifyValueChanged();
  }
  processForMessage(*msg, enumListeners);
  asyncNotifier.addMessage(msg);

};
void EnumParameter::modelOptionRemoved(EnumParameterModel *,Identifier & key) {
  auto msg = EnumChangeMessage::newStructureChangeMessage(key, false);
  if(getSelectedIds().contains(key)){
    msg->isSelectionChange = true;
    msg->isSelected = false;
    Parameter::notifyValueChanged();
  }
  processForMessage(*msg, enumListeners);
  asyncNotifier.addMessage(msg);
};

void EnumParameter::processForMessage(const EnumChangeMessage &msg,ListenerList<EnumListener> & _listeners){
  if(msg.isStructureChange){
    if(msg.isAdded){
      _listeners.call(&EnumListener::enumOptionAdded, this, msg.key);
    }
    else{
      _listeners.call(&EnumListener::enumOptionRemoved, this, msg.key);
    }

  }
  if (msg.isSelectionChange){
    // check validity state has not changed
    jassert(msg.isValid==getModel()->isValidId(msg.key));
    _listeners.call(&EnumListener::enumOptionSelectionChanged, this,msg.isSelected,getModel()->isValidId(msg.key), msg.key);

  }
}

void EnumParameter::newMessage(const EnumChangeMessage &msg) {
  processForMessage(msg, asyncEnumListeners);

};

String EnumParameter::stringValue() const{
  auto  selected =  getSelectedIds();
  if(selected.size()==0){
    return "";
  }
  else if(selected.size()==1){
    return selected[0].toString();
  }
  else{
    String res = selected[0].toString();
    for(int i = 1 ; i < selected.size() ; i++){
      res+=","+selected[i].toString();
    }
    return res;
  }
}


///////////////
// EnumParameterModel



EnumParameterModel::EnumParameterModel(bool shouldSave):shouldSaveModel(shouldSave){
  userOptions = new DynamicObject();
}

EnumParameterModel::~EnumParameterModel(){
  masterReference.clear();
}


void EnumParameterModel::addOption(Identifier key, var data,bool userDefined){
  // we don't want to override existing
  jassert(!hasProperty(key));
  addOrSetOption(key, data,userDefined);

}

void EnumParameterModel::addOrSetOption(Identifier key, var data,bool userDefined){
  jassert(!(userDefined && shouldSaveModel));
  bool isNew = false;
  if(userDefined){
    isNew = !userOptions->hasProperty(key);
    userOptions->setProperty(key, data);
  }
  else{
    isNew = !hasProperty(key);
    setProperty(key, data);
  }
  if(isNew){
  listeners.call(&Listener::modelOptionAdded,this,key);
  }
}

void EnumParameterModel::removeOption(Identifier key)
{
  bool hadOption = false;
  if(hasProperty(key)){
    hadOption = hasProperty(key);
    removeProperty(key);
  }
  else{
    hadOption = userOptions->hasProperty(key);
    userOptions->removeProperty(key);
  }
  if(hadOption){
  listeners.call(&Listener::modelOptionRemoved,this,key);
  }

}

var EnumParameterModel::getValueForId(const Identifier &key){
  if(hasProperty(key)){
    return getProperty(key);
  }
  else{
    return userOptions->getProperty(key);
  }
}

bool EnumParameterModel::isValidId(Identifier key){
  return hasProperty(key) || userOptions->hasProperty(key);
}
const NamedValueSet EnumParameterModel::getProperties()noexcept {
  NamedValueSet res(DynamicObject::getProperties());
  for(auto k : userOptions->getProperties()){
    res.set(k.name, k.value);
  }
  return res;
}
DynamicObject * EnumParameterModel::getObject(){
  jassert(!(shouldSaveModel &&userOptions->getProperties().size()>0));
  if(shouldSaveModel){
    return this;
  }
  else{
    return userOptions;
  }
}



