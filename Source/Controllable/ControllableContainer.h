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


#ifndef CONTROLLABLECONTAINER_H_INCLUDED
#define CONTROLLABLECONTAINER_H_INCLUDED

#include "Controllable.h"


#include "../Utils/DebugHelpers.h"

class ControllableContainer;
class ParameterContainer;
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
  virtual void containerCleared(ControllableContainer * /*origin*/){}
};

class ControllableContainer

{
public:
  ControllableContainer(StringRef niceName);
  virtual ~ControllableContainer();
  void removeFromParent();
  void clearContainer();
  void setUserDefined(bool);

  void setCustomShortName(const String &_shortName);
  void setAutoShortName();
  bool hasCustomShortName;

  // to override
  virtual String const getNiceName() =0;
  virtual String setNiceName(const String &_niceName);


  String shortName;

  bool isUserDefined;
  bool skipControllableNameInAddress;


  Uuid uid;

  OwnedArray<Controllable,CriticalSection> controllables;
  
  Array<WeakReference<ControllableContainer>,CriticalSection  > controllableContainers;
  ControllableContainer * parentContainer;


  
  void removeControllable(Controllable * c);
  Controllable * getControllableByName(const String &name, bool searchNiceNameToo = false);

  ControllableContainer* addChildControllableContainer(ControllableContainer * container,bool notify=true);
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
  
  virtual Controllable * getControllableForAddress(String addressSplit, bool recursive = true, bool getNotExposed = false);
  virtual Controllable * getControllableForAddress(StringArray addressSplit, bool recursive = true, bool getNotExposed = false);
  bool containsControllable(Controllable * c, int maxSearchLevels = -1);
  String getControlAddress(ControllableContainer * relativeTo=nullptr);




  void dispatchFeedback(Controllable * c);

  
  
  


  // get non user-created custom parameter from JSON
  virtual void loadCustomJSONElement(const String & ,const var v){jassertfalse;};



//  virtual void childStructureChanged(ControllableContainer *notifier,ControllableContainer * origin)override;

  String getUniqueNameInContainer(const String &sourceName, int suffix = 0,void * me=nullptr);

  virtual ParameterContainer * getParameterContainer() = 0;

  

  int numContainerIndexed;
  int localIndexedPosition;



public:

  typedef ControllableContainerListener Listener ;
  
  
  ListenerList<ControllableContainerListener> controllableContainerListeners;
  void addControllableContainerListener(ControllableContainerListener* newListener) { controllableContainerListeners.add(newListener);}
  void removeControllableContainerListener(ControllableContainerListener* listener) { controllableContainerListeners.remove(listener);}






  protected :

  //  container with custom controllable can override this 
  virtual void addControllableInternal(Controllable *){};

  /// identifiers
  static const Identifier controlAddressIdentifier;
  static const Identifier childContainerId;
  static const Identifier controllablesId;
  friend class PresetManager;

  void notifyStructureChanged(ControllableContainer * origin);
protected:

  WeakReference< ControllableContainer >::Master masterReference;
  friend class WeakReference<ControllableContainer>;
  






  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ControllableContainer)


};





#endif  // CONTROLLABLECONTAINER_H_INCLUDED
