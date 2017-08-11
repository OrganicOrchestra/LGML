/*
 ==============================================================================

 EnumParameter.cpp
 Created: 29 Sep 2016 5:34:59pm
 Author:  bkupe

 ==============================================================================
 */

#include "EnumParameter.h"



Identifier EnumParameter::modelIdentifier("model");
Identifier EnumParameter::selectedSetIdentifier("selected");

///////////////////
// EnumParameter

EnumParameter::EnumParameter(const String & niceName, const String &description, EnumParameterModel * modelInstance,bool _userCanEnterText, bool enabled) :
Parameter(Type::ENUM, niceName, description,var(),var(),var(), enabled),
asyncNotifier(1000),
userCanEnterText(_userCanEnterText)
{
  enumData = new DynamicObject();
  enumData->setProperty(selectedSetIdentifier, Array<var>());
  ownModel = modelInstance==nullptr;
  model = ownModel?new EnumParameterModel():modelInstance;
  if(ownModel) {
    enumData->setProperty(modelIdentifier, model.get() );
  }

  model->listeners.add(this);

  asyncNotifier.addListener(this);

  value = var(enumData);

}
EnumParameter::~EnumParameter(){
  //  value = var::null;
  asyncNotifier.removeListener(this);
  enumData=nullptr;
}

EnumParameterModel * EnumParameter::getModel() const{
  jassert(model.get());
  return model.get();

}

void EnumParameter::addOption(Identifier key, var data)
{
  //  adding option thru parameter is not supported when using a shared model
  jassert(ownModel || userCanEnterText);
  getModel()->addOption(key,data);
}

void EnumParameter::addOrSetOption(Identifier key, var data)
{
  //  adding option thru parameter is not supported when using a shared model
  jassert(ownModel|| userCanEnterText);
  getModel()->addOrSetOption(key,data);
}
void EnumParameter::removeOption(Identifier key)
{
  //  removing option thru parameter is not supported when using a shared model
  jassert(ownModel);
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
    if(! (oldS.size()==1 && oldS.getReference(0) == key) ) {
     unselectAll();
    }
  }
  Array<var> * selection = getSelectedSet(value);
  jassert(selection);
  //  if(!appendSelection)selection->clear();
  int numSelectionChange = 0;
  if(shouldSelect  && !selection->contains(key.toString())){
    selection->add(key.toString());
    numSelectionChange = 1;
  }
  else{
    numSelectionChange = selection->removeAllInstancesOf(key.toString());

  }
  if(numSelectionChange>0){
    auto msg = EnumChangeMessage::newSelectionMessage(key, shouldSelect, getModel()->isValidId(key));
    processForMessage(*msg, enumListeners);
    asyncNotifier.addMessage(msg);

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
  DynamicObject * vm = getModel();
  for(auto & i: getSelectedSetIds(value)){
    res.add( vm->getProperty(i));
  }
  return res;
};

var EnumParameter::getValueForId(const Identifier &i){
  return getModel()->getValueForId(i);
}



void EnumParameter::setValueInternal(var & _value){

  if(selectFromVar(_value,true,false)){}
  else if(_value.isArray()){
    unselectAll();
    for(auto &v : *_value.getArray()){
      jassert(selectFromVar(v,true,true));
    }
  }

  // rebuild the whole model if needed and select
  else if(DynamicObject * dvalues = _value.getProperty(modelIdentifier,var::null).getDynamicObject()){

    // if model is stored, this param should own it

    jassert(ownModel);
    if (ownModel){

      if(DynamicObject * oldValues = getModel()){
        auto oldP = oldValues->getProperties();
        for(auto v:oldP){
          removeOption(v.name);
        }

      }
      for(auto v:dvalues->getProperties()){
        addOption(v.name.toString(), v.value);
      }

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
  }
  processForMessage(*msg, enumListeners);
  asyncNotifier.addMessage(msg);

};
void EnumParameter::modelOptionRemoved(EnumParameterModel *,Identifier & key) {
  auto msg = EnumChangeMessage::newStructureChangeMessage(key, false);
  if(getSelectedIds().contains(key)){
    msg->isSelectionChange = true;
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
    _listeners.call(&EnumListener::enumOptionSelectionChanged, this,msg.isSelected,msg.isValid, msg.key);
  }
}

void EnumParameter::newMessage(const EnumChangeMessage &msg) {
  processForMessage(msg, asyncEnumListeners);

};

String EnumParameter::stringValue() const{
  auto  selected =  getSelectedValues();
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



