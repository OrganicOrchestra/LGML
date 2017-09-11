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


#ifndef CONTROLLABLE_H_INCLUDED
#define CONTROLLABLE_H_INCLUDED


#include "JuceHeader.h"//keep

class ControllableContainer;
class ControllableUI;
class ControllableContainer;
class Parameter;

class Controllable
{
public:
  enum Type { //Add type here if creating new type of Controllable
    CUSTOM, // allow opaque controllables with custom editors
    //    //(mainly to avoid polluting this enum when using custom editors)
    TRIGGER,
    FLOAT,
    INT,
    BOOL,
    STRING,
    ENUM,
    POINT2D,
    POINT3D,
    RANGE,
    PROXY,


  };


  Controllable(const Type &type, const String &niceName, const String &description, bool enabled = true);
  virtual ~Controllable();


  Type type;
  String niceName;
  String shortName;
  String description;


  bool enabled;
  bool hasCustomShortName;
  bool isControllableExposed;
  bool hideInEditor;
  bool shouldSaveObject;
  bool isUserDefined;
  bool isSavable;
  String controlAddress;


  ControllableContainer * parentContainer;

  void setNiceName(const String &_niceName);
  void setCustomShortName(const String &_shortName);
  void setAutoShortName();

  void setEnabled(bool value, bool silentSet = false, bool force = false);

  void setParentContainer(ControllableContainer * container);
  bool isChildOf(ControllableContainer * p);
  void updateControlAddress();

  String getControlAddress(ControllableContainer * relativeTo = nullptr);

  
  virtual bool isMappable();


  //used for script variables
  virtual DynamicObject * createDynamicObject();
  static var getVarStateFromScript(const juce::var::NativeFunctionArgs & a);

  virtual var getVarObject()=0;
  virtual var getVarState() = 0;
  virtual var setVarState(const var & v) = 0;
  virtual void setFromVarObject(DynamicObject & ob) = 0;


  // using this as a compile time check that no controllable is parameter
  // will remove this soon when inheritance parameter / controllable is sorted out
  virtual Parameter* getParameter() = 0;




public:

  class  Listener
  {
  public:
    /** Destructor. */
    virtual ~Listener() {}
    virtual void controllableStateChanged(Controllable * ) {};
    virtual void controllableControlAddressChanged(Controllable * ) {};
    virtual void controllableNameChanged(Controllable *) {};
    virtual void controllableRemoved(Controllable * ) {};
  };


  ListenerList<Listener> listeners;
  void addControllableListener(Listener* newListener) { listeners.add(newListener); }
  void removeControllableListener(Listener* listener) { listeners.remove(listener); }


  //Script set method handling
  static var setControllableValueFromJS(const juce::var::NativeFunctionArgs& a);

  
private:

  WeakReference<Controllable>::Master masterReference;
  friend class WeakReference<Controllable>;



  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Controllable)
};

#endif  // CONTROLLABLE_H_INCLUDED
