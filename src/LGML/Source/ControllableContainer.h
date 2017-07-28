/*
 ==============================================================================

 ControllableContainer.h
 Created: 8 Mar 2016 1:15:36pm
 Author:  bkupe

 ==============================================================================
 */

#ifndef CONTROLLABLECONTAINER_H_INCLUDED
#define CONTROLLABLECONTAINER_H_INCLUDED

#include "Controllable.h"
#include "FloatParameter.h" //keep
#include "IntParameter.h" //keep
#include "BoolParameter.h" //keep
#include "StringParameter.h" //keep
#include "EnumParameter.h"
#include "Point2DParameter.h"
#include "Point3DParameter.h"

#include "Trigger.h"
#include "PresetManager.h"
#include "DebugHelpers.h" //keep

class ControllableContainer;

//Listener
class  ControllableContainerListener
{
public:
  /** Destructor. */
  virtual ~ControllableContainerListener() {}
  virtual void controllableAdded(ControllableContainer *,Controllable *) {}
  virtual void controllableRemoved(ControllableContainer *,Controllable *) {}
  virtual void controllableContainerAdded(ControllableContainer *,ControllableContainer *) {}
  virtual void controllableContainerRemoved(ControllableContainer *,ControllableContainer *) {}
  virtual void controllableFeedbackUpdate(ControllableContainer *,Controllable *) {}
  virtual void childStructureChanged(ControllableContainer * /*notifier*/,ControllableContainer* /*origin*/) {}
  virtual void childAddressChanged(ControllableContainer * ){};
  virtual void controllableContainerPresetLoaded(ControllableContainer *) {}
};

class CCInnerContainer;
class ControllableContainer : public Parameter::Listener,public Parameter::AsyncListener, public ControllableContainerListener

{
public:
  ControllableContainer(const String &niceName,bool isUserDefined=false);
  virtual ~ControllableContainer();
  void remove();
  bool isUserDefined;
  const String  getNiceName();
  String shortName;
  bool hasCustomShortName;

  bool canHavePresets;
  bool presetSavingIsRecursive;
  StringParameter * currentPresetName,*nameParam;
  Trigger * savePresetTrigger;
  PresetManager::Preset * currentPreset;


  bool skipControllableNameInAddress;
  void setNiceName(const String &_niceName);
  void setCustomShortName(const String &_shortName);
  void setAutoShortName();

  Uuid uid;

  OwnedArray<Controllable,CriticalSection> controllables;
  
  Array<WeakReference<ControllableContainer>,CriticalSection  > controllableContainers;
  ControllableContainer * parentContainer;

  template<class T,class... Args>
  T* addNewParameter(const String & _niceName,const String & desc,Args...args);

  template<class T,class... Args>
  T* addNewUserParameter(const Identifier & id,const String & _niceName,const String & desc,Args...args);
  void removeUserParameter(const Identifier & id,Parameter *const *el);

  typedef Array<Parameter*> UsrParameterList;
  HashMap<int64,Array<Parameter*>*> userParameterMap;

  UsrParameterList * getUserParameters(const Identifier & i);
  Parameter *  getUserParameter(const Identifier & id,const String & niceName);
  UsrParameterList getAllUserParameters();
  
  Parameter* addParameter(Parameter * );


  
  void removeControllable(Controllable * c);
  Controllable * getControllableByName(const String &name, bool searchNiceNameToo = false);

  void addChildControllableContainer(ControllableContainer * container);
  void removeChildControllableContainer(ControllableContainer *container);
  // add indexed container (ensure localIndex and position in the child container array are the same)
  // idx of -1 add after the ast indexed (may be not the last, array can contain other non indexed elements)
  void addChildIndexedControllableContainer(ControllableContainer * container,int idx = -1);
  void removeChildIndexedControllableContainer(int idx);
  int getNumberOfIndexedContainer();
  int getIndexedPosition();
  bool hasIndexedContainers();
  bool isIndexedContainer();
  // can be overriden if indexed container are removed from the middle of the list,
  // allowing Indexed containers to react to index change
  virtual void localIndexChanged();
  template<class T>
  Array<T*> getObjectsOfType(bool recursive = false);

  bool containsContainer(ControllableContainer * );

  ControllableContainer * getControllableContainerByName(const String &name, bool searchNiceNameToo = false);
  ControllableContainer * getControllableContainerForAddress( StringArray  address);

  void setParentContainer(ControllableContainer * container);
  void updateChildrenControlAddress();


  virtual Array<WeakReference<Controllable>> getAllControllables(bool recursive = false, bool getNotExposed = false);
  virtual Array<WeakReference<ControllableContainer>> getAllControllableContainers(bool recursive = false);
  virtual Array<WeakReference<Parameter>> getAllParameters(bool recursive = false, bool getNotExposed = false);
  virtual Controllable * getControllableForAddress(String addressSplit, bool recursive = true, bool getNotExposed = false);
  virtual Controllable * getControllableForAddress(StringArray addressSplit, bool recursive = true, bool getNotExposed = false);
  bool containsControllable(Controllable * c, int maxSearchLevels = -1);
  String getControlAddress(ControllableContainer * relativeTo=nullptr);


  virtual bool loadPresetWithName(const String &name);
  virtual bool loadPreset(PresetManager::Preset * preset);
  virtual PresetManager::Preset* saveNewPreset(const String &name);
  virtual bool saveCurrentPreset();
  virtual int getNumPresets();

  virtual bool resetFromPreset();

  //    to be overriden
  virtual void loadPresetInternal(PresetManager::Preset *){};
  virtual void savePresetInternal(PresetManager::Preset *){};

  void cleanUpPresets();

  virtual String getPresetFilter();
  virtual var getPresetValueFor(Parameter * p);//Any parameter that is part of a this preset can use this function

  void dispatchFeedback(Controllable * c);

  // Inherited via Parameter::Listener
  virtual void parameterValueChanged(Parameter * p) override;



  
  virtual var getJSONData();
  virtual void loadJSONData(var data);
  virtual void loadJSONDataInternal(var /*data*/) { /* to be overriden by child classes */ }
  // get non user-created custom parameter from JSON
  virtual void loadCustomJSONElement(const Identifier & name,const var v){jassertfalse;};



  virtual void childStructureChanged(ControllableContainer *notifier,ControllableContainer * origin)override;

  String getUniqueNameInContainer(const String &sourceName, int suffix = 0);


  virtual Component * getCustomEditor(){return nullptr;}
private:
  // internal callback that a controllableContainer can override to react to any of it's parameter change
  //@ ben this is to avoid either:
  //      adding controllableContainerListener for each implementation
  //      or overriding parameterValueChanged and needing to call ControllableContainer::parameterValueChanged in implementation (it should stay independent as a different mechanism)
  //      or using dispatch feedback that triggers only exposedParams

  virtual void onContainerParameterChanged(Parameter *) {};
  virtual void onContainerTriggerTriggered(Trigger *) {};
  virtual void onContainerParameterChangedAsync(Parameter *,const var & /*value*/){};
  

  int numContainerIndexed;
  int localIndexedPosition;


  // anti feedback when loading preset ( see loadPresetWithName)
  bool isLoadingPreset = false;

public:


  ListenerList<ControllableContainerListener> controllableContainerListeners;
  void addControllableContainerListener(ControllableContainerListener* newListener) { controllableContainerListeners.add(newListener);}
  void removeControllableContainerListener(ControllableContainerListener* listener) { controllableContainerListeners.remove(listener);}
  void clear();





  protected :

  //  container with custom controllable can override this 
  virtual void addControllableInternal(Controllable *c){};

  /// identifiers

  static const Identifier presetIdentifier;

  static const Identifier controlAddressIdentifier;
  static const Identifier valueIdentifier;
  static const Identifier paramsIdentifier;
  static const Identifier userParamIdentifier;
  static const Identifier containerNameIdentifier;
  static const Identifier childContainerId;


private:


  WeakReference<ControllableContainer>::Master masterReference;
    friend class WeakReference<ControllableContainer>;
  void notifyStructureChanged(ControllableContainer * origin);
  void newMessage(const  Parameter::ParamWithValue&)override;





  friend class PresetManager;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ControllableContainer)


};

/// templates



template<class T,class... Args>
T* ControllableContainer::addNewParameter(const String & _niceName,const String & desc,Args...args)
{

  String targetName = getUniqueNameInContainer(_niceName);
  T* p = new T(targetName,desc,args...);
  return dynamic_cast<T*>(addParameter(p));


}
template<class T,class... Args>
T* ControllableContainer::addNewUserParameter(const Identifier & id,const String & _niceName,const String & desc,Args...args){

  String targetName = getUniqueNameInContainer(_niceName);
  auto p = new T(targetName,desc,args...);
  p->shouldSaveObject = true;
  p->isUserDefined = true;
  int64 key = (int64)(id.getCharPointer().getAddress());
  if(!userParameterMap.contains(key)){
    userParameterMap.set(key,new UsrParameterList());
  }
  userParameterMap[key]->add(p);

  addParameter(p);
  return p;


}






#endif  // CONTROLLABLECONTAINER_H_INCLUDED
