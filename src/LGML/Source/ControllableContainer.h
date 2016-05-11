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
#include "FloatParameter.h"
#include "IntParameter.h"
#include "BoolParameter.h"
#include "StringParameter.h"
#include "Trigger.h"
#include "PresetManager.h"

class ControllableContainerEditor;

class ControllableContainer : public Parameter::Listener, public Trigger::Listener
{
public:
    ControllableContainer(const String &niceName);
    virtual ~ControllableContainer();

    String niceName;
    String shortName;
    bool hasCustomShortName;

    bool skipControllableNameInAddress;

    void setNiceName(const String &_niceName);

    void setCustomShortName(const String &_shortName);

    void setAutoShortName();


    OwnedArray<Controllable> controllables;
    Array<ControllableContainer * > controllableContainers;
    ControllableContainer * parentContainer;

	void addParameter(Parameter * p);
    FloatParameter * addFloatParameter(const String &niceName, const String &description, const float &initialValue, const float &minValue = 0, const float &maxValue = 1, const bool &enabled = true);
    IntParameter * addIntParameter(const String &niceName, const String &description, const int &initialValue, const int &minValue, const int &maxValue, const bool &enabled = true);
    BoolParameter * addBoolParameter(const String &niceName, const String &description, const bool &value, const bool &enabled = true);
    StringParameter * addStringParameter(const String &niceName, const String &description, const String &value, const bool &enabled = true);
    Trigger * addTrigger(const String &niceName, const String &description, const bool &enabled = true);

    void removeControllable(Controllable * c);
    Controllable * getControllableByName(const String &name);

    void addChildControllableContainer(ControllableContainer * container);
    void removeChildControllableContainer(ControllableContainer *container);
    ControllableContainer * getControllableContainerByName(const String &name);


    void setParentContainer(ControllableContainer * container);
    void updateChildrenControlAddress();
    virtual Array<Controllable *> getAllControllables(bool recursive = false, bool getNotExposed = false);
    virtual Array<Parameter *> getAllParameters(bool recursive = false, bool getNotExposed = false);

    virtual Controllable * getControllableForAddress(String addressSplit, bool recursive = true, bool getNotExposed = false);

    virtual Controllable * getControllableForAddress(StringArray addressSplit, bool recursive = true, bool getNotExposed = false);


    PresetManager::Preset * currentPreset;
    virtual bool loadPreset(PresetManager::Preset * preset);
	virtual void saveNewPreset(const String &name);
    virtual bool saveCurrentPreset();
    virtual bool resetFromPreset();
	virtual String getPresetFilter();
	virtual var getPresetValueFor(Parameter * p);//Any parameter that is part of a this preset can use this function

    void dispatchFeedback(Controllable * c);

    // Inherited via Parameter::Listener
    virtual void parameterValueChanged(Parameter * p) override;
    // Inherited via Trigger::Listener
    virtual void triggerTriggered(Trigger * p) override;

	virtual var getJSONData();
	virtual void loadJSONData(var data);
	virtual void loadJSONDataInternal(var data) { /* to be overriden by child classes */ }





private:
    // internal callback that a controllableContainer can override to react to any of it's parameter change
    //@ ben this is to avoid either:
    //      adding controllableContainerListener for each implementation
    //      or overriding parameterValueChanged and needing to call ControllableContainer::parameterValueChanged in implementation (it should stay independent as a different mechanism)
    //      or using dispatch feedback that triggers only exposedParams

    virtual void onContainerParameterChanged(Parameter *) {};
    virtual void onContainerTriggerTriggered(Trigger *) {};
    void addParameterInternal(Parameter * p);

public:
    //Listener
    class  Listener
    {
    public:
        /** Destructor. */
        virtual ~Listener() {}
		virtual void controllableAdded(Controllable * ) {}
        virtual void controllableRemoved(Controllable * ) {}
        virtual void controllableContainerAdded(ControllableContainer *) {}
        virtual void controllableContainerRemoved(ControllableContainer * ) {}
        virtual void controllableFeedbackUpdate(Controllable * ) {}
        virtual void childStructureChanged(ControllableContainer * ){}
		virtual void controllableContainerPresetLoaded(ControllableContainer * ) {}
    };

    ListenerList<Listener> controllableContainerListeners;
    void addControllableContainerListener(Listener* newListener) { controllableContainerListeners.add(newListener);}
    void removeControllableContainerListener(Listener* listener) { controllableContainerListeners.remove(listener);}


private:


    void notifyStructureChanged();

    WeakReference<ControllableContainer>::Master masterReference;
    friend class WeakReference<ControllableContainer>;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ControllableContainer)


};


#endif  // CONTROLLABLECONTAINER_H_INCLUDED
