/*
 ==============================================================================

 ParameterProxy.cpp
 Created: 31 May 2016 12:28:42pm
 Author:  bkupe

 ==============================================================================
 */

#include "ParameterProxy.h"
#include "ControllableContainer.h"
#include "Engine.h"

ParameterProxy::ParameterProxy(const String & niceName,const String & desc,Parameter * ref,ControllableContainer * root) :
StringParameter(niceName,desc),
linkedParam(ref),
rootOfProxy(nullptr)
{
  type = Controllable::PROXY;
  setRoot(root);


}

ParameterProxy::~ParameterProxy()
{
  if(auto r =getRoot())r->removeControllableContainerListener(this);
  if (linkedParam != nullptr){
    linkedParam->removeControllableListener(this);
    linkedParam->removeParameterListener(this);
  }
}


void ParameterProxy::setRoot(ControllableContainer * r){
  if(rootOfProxy!=nullptr)rootOfProxy->removeControllableContainerListener(this);
  rootOfProxy = r;
  resolveAddress();

}
void ParameterProxy::tryToSetValue(var _value,bool silentSet,bool force ){

  if(_value.isString()){
    StringParameter::tryToSetValue(_value, silentSet, force);
  }
  else if (linkedParam){
    //WIP : polymorphic set value not supported
    jassertfalse;
    linkedParam->tryToSetValue(_value, silentSet, force);
  }
};
void ParameterProxy::setValueInternal(var & _value)
{
  if(_value.isString()){
  StringParameter::setValueInternal(_value);
  if(auto * root = getRoot()){
    if(!resolveAddress() && stringValue().isNotEmpty()){
      root->addControllableContainerListener(this);
    }
    else{
      root->removeControllableContainerListener(this);
    }
  }
  }
  else if(linkedParam){
    //WIP : polymorphic set value not supported
    jassertfalse;
    linkedParam->setValueInternal(_value);
  }
}


void ParameterProxy::parameterValueChanged(Parameter * p)
{
  jassert(p==linkedParam);
  proxyListeners.call(&ParameterProxyListener::linkedParamValueChanged, this);
}

Parameter* ParameterProxy::get(){
  return linkedParam.get();
}
void ParameterProxy::setParamToReferTo(Parameter * p)
{

  String targetAddress = p?p->getControlAddress(getRoot()):String::empty;
  if( targetAddress!=stringValue()){
    setValue(targetAddress);
  }
  else{
    if (linkedParam != nullptr){
      linkedParam->removeParameterListener(this);
      linkedParam->removeControllableListener(this);
    }
    if(p==this){
      jassertfalse;
      DBG("try to auto reference proxy");
      linkedParam = nullptr;
    }
    else{
      linkedParam = p;
    }
    if (linkedParam != nullptr){
      linkedParam->addParameterListener(this);
      linkedParam->addControllableListener(this);
    }

    proxyListeners.call(&ParameterProxyListener::linkedParamChanged,this);
  }
}



ControllableContainer * ParameterProxy::getRoot(){
  return (rootOfProxy?rootOfProxy:getEngine());
}


bool ParameterProxy::resolveAddress(){
  if(stringValue().isNotEmpty()){
    auto p = getRoot()->getControllableForAddress(stringValue())->getParameter();

    setParamToReferTo(p);
  }
  else{
    setParamToReferTo(nullptr);
  }
  return linkedParam!=nullptr;
}

void ParameterProxy::controllableAdded(ControllableContainer *,Controllable * c) {
  jassert(linkedParam==nullptr);
  if(c->getControlAddress()==stringValue()){
    setParamToReferTo(c->getParameter());
  }
  
}

void ParameterProxy::controllableRemoved(Controllable * c) {
  if(c==(Controllable*)linkedParam || !linkedParam.get()){
    setParamToReferTo(nullptr);
  }

};
