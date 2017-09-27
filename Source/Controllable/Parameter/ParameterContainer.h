/*
 ==============================================================================

 ParameterContainer.h
 Created: 5 Sep 2017 3:59:42pm
 Author:  Martin Hermant

 ==============================================================================
 */

#pragma once
#include "../ControllableContainer.h"
// PARAMETER

#include "Parameter.h"

#include "../Parameter/NumericParameter.h"
#include "../Parameter/BoolParameter.h"
#include "../Parameter/StringParameter.h"
#include "../Parameter/EnumParameter.h"
#include "../Parameter/RangeParameter.h"
#include "../Parameter/Point2DParameter.h"
#include "../Parameter/Point3DParameter.h"
#include "../Parameter/Trigger.h"

#include "../../Preset/PresetManager.h"
#include "../../Utils/FactoryObject.h"



namespace juce {class Component;};
class StringParameter;
class Trigger;
class ParameterContainer: public ControllableContainer,
    public Parameter::Listener,
    public Parameter::AsyncListener,
    public ControllableContainer::Listener,
    FactoryObject
{
public:
    DECLARE_OBJ_TYPE (ParameterContainer)
    virtual ~ParameterContainer();

    template<class T, class... Args>
    T* addNewParameter (const String& _niceName, const String& desc, Args...args);
    StringParameter* nameParam;
    String const getNiceName() override;
    String setNiceName (const String& _niceName) override;



    virtual ParameterContainer* addContainerFromObject (const String& name, DynamicObject*   data) ;
    virtual Parameter* addParameterFromVar (const String& name, const var& data) ;

    virtual void configureFromObject (DynamicObject* data) override;
    virtual DynamicObject* getObject() override;
    virtual juce::Component* getCustomEditor() {return nullptr;}

    //  controllableContainer::Listener
    virtual void controllableRemoved (ControllableContainer*, Controllable*) override;
    virtual void containerCleared (ControllableContainer* ) override;



    //////////////////
    //// preset

    virtual bool loadPresetWithName (const String& name);
    virtual bool loadPreset (PresetManager::Preset* preset);
    virtual PresetManager::Preset* saveNewPreset (const String& name);
    virtual bool saveCurrentPreset();
    virtual int getNumPresets();


    virtual bool resetFromPreset();
    static const Identifier presetIdentifier;
    static const Identifier uidIdentifier;


    //    to be overriden
    virtual void loadPresetInternal (PresetManager::Preset*) {};
    virtual void savePresetInternal (PresetManager::Preset*) {};

    void cleanUpPresets();

    virtual String getPresetFilter();
    virtual var getPresetValueFor (Parameter* p);//Any parameter that is part of a this preset can use this function


    void setUserDefined (bool v);

    Parameter* addParameter (Parameter* );
    Array<WeakReference<Parameter>> getAllParameters (bool recursive = false, bool getNotExposed = false);

    // Inherited via Parameter::Listener
    virtual void parameterValueChanged (Parameter* p) override;
    void newMessage (const  Parameter::ParamWithValue&)override;

    bool canHavePresets;
    bool presetSavingIsRecursive;
    StringParameter* currentPresetName;
    Trigger* savePresetTrigger;
    PresetManager::Preset* currentPreset;



    // anti feedback when loading preset ( see loadPresetWithName)
    bool isLoadingPreset = false;
    friend class PresetManager;

    WeakReference< ParameterContainer >::Master masterReference;
    friend class WeakReference<ParameterContainer>;


private:
    // internal callback that a controllableContainer can override to react to any of it's parameter change
    //@ ben this is to avoid either:
    //      adding controllableContainerListener for each implementation
    //      or overriding parameterValueChanged and needing to call ControllableContainer::parameterValueChanged in implementation (it should stay independent as a different mechanism)
    //      or using dispatch feedback that triggers only exposedParams

    virtual void onContainerParameterChanged (Parameter*) {};
    virtual void onContainerTriggerTriggered (Trigger*) {};
    virtual void onContainerParameterChangedAsync (Parameter*, const var& /*value*/) {};


};
/// templates



template<class T, class... Args>
T* ParameterContainer::addNewParameter (const String& _niceName, const String& desc, Args...args)
{

    String targetName = getUniqueNameInContainer (_niceName);
    T* p = new T (targetName, desc, args...);
    p->resetValue (true);
    return static_cast<T*> (addParameter (p));


}
