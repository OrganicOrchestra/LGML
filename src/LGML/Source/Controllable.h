/*
 ==============================================================================

 Controllable.h
 Created: 8 Mar 2016 1:08:56pm
 Author:  bkupe

 ==============================================================================
 */

#ifndef CONTROLLABLE_H_INCLUDED
#define CONTROLLABLE_H_INCLUDED


#include "JuceHeader.h"//keep

class ControllableContainer;
class ControllableUI;
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
  bool isControllableFeedbackOnly;
  bool hideInEditor;
  bool shouldSaveObject;
  bool isUserDefined;
  bool isSavable;
  String controlAddress;

  bool replaceSlashesInShortName;

  ControllableContainer * parentContainer;

  void setNiceName(const String &_niceName);
  void setCustomShortName(const String &_shortName);
  void setAutoShortName();

  void setEnabled(bool value, bool silentSet = false, bool force = false);

  void setParentContainer(ControllableContainer * container);
  void updateControlAddress();

  String getControlAddress(ControllableContainer * relativeTo = nullptr);

  virtual bool isNumeric(); //default implementation, can be overriden for special parameters



  //used for script variables
  virtual DynamicObject * createDynamicObject();
  static var getVarStateFromScript(const juce::var::NativeFunctionArgs & a);


  virtual var getVarObject()=0;
  virtual var getVarState() = 0;
  virtual void setFromVarObject(DynamicObject & ob) = 0;
  virtual  const Identifier  getTypeIdentifier();
  // helper identifier to use when overiding
  static const Identifier varTypeIdentifier;



  Parameter* getParameter();




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
