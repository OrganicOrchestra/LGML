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

// base class for model behind an enum parameter
// a model is a dictionary of var notifying its structural changes
// it can be used to share it's content to different instances of enum parameter
class EnumParameterModel : public DynamicObject{
public:

  EnumParameterModel(){
  }
  virtual ~EnumParameterModel(){
    masterReference.clear();
  }


  void addOption(Identifier key, var data){
    // we don't want to override existing
    jassert(!hasProperty(key));
    addOrSetOption(key, data);

  }

  void addOrSetOption(Identifier key, var data){
    setProperty(key, data);
    listeners.call(&Listener::modelOptionAdded,this,key);
  }

  void removeOption(Identifier key)
  {
    removeProperty(key);
    listeners.call(&Listener::modelOptionRemoved,this,key);


  }
  var getValueForId(const Identifier &i){
    return getProperty(i);
  }

  bool isValidId(Identifier key){
    return hasProperty(key);
  }



  class Listener{
  public:
    virtual ~Listener(){};
    virtual void modelOptionAdded(EnumParameterModel *,Identifier &)=0;
    virtual void modelOptionRemoved(EnumParameterModel *,Identifier &) = 0;
  };
  ListenerList<Listener> listeners;

private:

  WeakReference<EnumParameterModel>::Master masterReference;
  friend class WeakReference<EnumParameterModel>;

};

class EnumChangeMessage{
  static EnumChangeMessage * newStructureChangeMessage(const Identifier & k,bool isAdded){
    auto msg = new EnumChangeMessage();
    msg->key = k;
    msg->isStructureChange = true;
    msg->isAdded = isAdded;
    return msg;
  }

  static EnumChangeMessage * newSelectionMessage(const Identifier & k, bool isSelected,bool isValid){
    auto msg = new EnumChangeMessage();
    msg->key = k;
    msg->isStructureChange = false;
    msg->isValid = isValid;
    msg->isSelected = isSelected;
    return msg;
  }

  Identifier key;
  bool isStructureChange;
  bool isAdded;
  bool isSelected;
  bool isValid;
  friend class EnumParameter;
};


// EnumParameter changes are deffered to message thread per default
class EnumParameter : public Parameter,public EnumParameterModel::Listener,public QueuedNotifier<EnumChangeMessage>::Listener
{
public:
  EnumParameter(const String &niceName, const String &description, EnumParameterModel * modelInstance=nullptr, bool enabled = true);
  ~EnumParameter() ;
  
  static Identifier selectedSetIdentifier;
  static Identifier modelIdentifier;
  void addOption(Identifier key, var data=var::null);
  void addOrSetOption(Identifier key, var data=var::null);
  void removeOption(Identifier key);
  void selectId(Identifier key,bool shouldSelect,bool appendSelection = true);
  bool selectFromVar(var & _value,bool shouldSelect,bool appendSelection=true);
  void unselectAll();



  void setValueInternal(var & _value) override;


  Array<Identifier> getSelectedIds() ;
  Identifier getFirstSelectedId() ;

  Array<var> getSelectedValues();
  var getFirstSelectedValue(var defaultValue=var::null) ;
  bool selectionIsNotEmpty();
  var getValueForId(const Identifier &i);
  EnumParameterUI * createUI(EnumParameter * target = nullptr);
  ControllableUI * createDefaultUI(Controllable * targetControllable = nullptr) override;


  //Listener
  // per parameter Instance
  class  EnumListener
  {
  public:
    /** Destructor. */
    virtual ~EnumListener() {}

    virtual void enumOptionAdded(EnumParameter *, const Identifier &) = 0;
    virtual void enumOptionRemoved(EnumParameter *, const Identifier &) = 0;
    virtual void enumOptionSelectionChanged(EnumParameter *,bool /*isSelected*/, bool /*isValid*/, const Identifier &){};
  };


  QueuedNotifier<EnumChangeMessage> asyncNotifier;
  void newMessage(const EnumChangeMessage &msg) override;

  ListenerList<EnumListener> enumListeners,asyncEnumListeners;
  void addEnumParameterListener(EnumListener* newListener) { enumListeners.add(newListener); }
  void removeEnumParameterListener(EnumListener* listener) { enumListeners.remove(listener); }

  void addAsyncEnumParameterListener(EnumListener* newListener) { asyncEnumListeners.add(newListener); }
  void removeAsyncEnumParameterListener(EnumListener* listener) { asyncEnumListeners.remove(listener); }
  EnumParameterModel * getModel();

private:
  DynamicObject * getValuesMap(const var & v);
  Array<Identifier> getSelectedSetIds(const var & v);
  Array<var> *getSelectedSet(const var & v);

  WeakReference<EnumParameterModel> model;
  DynamicObject * enumData;
  bool ownModel;

  // model Listener
  void modelOptionAdded(EnumParameterModel *,Identifier &) override;
  void modelOptionRemoved(EnumParameterModel *,Identifier &) override;

  void processForMessage(const EnumChangeMessage &,ListenerList<EnumListener> & _listeners);

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EnumParameter)
};


#endif  // ENUMPARAMETER_H_INCLUDED
