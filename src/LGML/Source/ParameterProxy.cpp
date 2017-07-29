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
isUpdatingLinkedParam(false)
{
  setRoot(root);
  Parameter::isEditable = false;

}

ParameterProxy::~ParameterProxy()
{
  if(auto r =getRoot())r->removeControllableContainerListener(this);

}


void ParameterProxy::setRoot(ControllableContainer * r){
  rootOfProxy = r;
  resolveAddress();

}
void ParameterProxy::setValueInternal(var & _value)
{
  StringParameter::setValueInternal(_value);

  if(!resolveAddress() && stringValue().isNotEmpty())
    getRoot()->addControllableContainerListener(this);
  else
    getRoot()->removeControllableContainerListener(this);
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
    if (linkedParam == p) return;
    if (linkedParam != nullptr)
    {
      linkedParam->removeParameterListener(this);
    }
    linkedParam = p;

    if (linkedParam != nullptr)
    {
      linkedParam->addParameterListener(this);

    }

    proxyListeners.call(&ParameterProxyListener::linkedParamChanged,this);
  }
}



ControllableContainer * ParameterProxy::getRoot(){
  return (rootOfProxy?rootOfProxy:getEngine());
}


bool ParameterProxy::resolveAddress(){
  if(stringValue().isNotEmpty()){
    auto p = dynamic_cast<Parameter*>(getRoot()->getControllableForAddress(stringValue()));

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
