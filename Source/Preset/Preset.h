/*
  ==============================================================================

    Preset.h
    Created: 8 Dec 2018 8:38:36pm
    Author:  Martin Hermant

  ==============================================================================
*/

#pragma once

#include "../JuceHeaderCore.h"
#include "../Controllable/Parameter/ParameterContainer.h"


class Preset : public ParameterContainer
{
public:

    Preset (const String& _name, ParameterContainer * pc);
    ~Preset(){masterReference.clear();}

    String getPresetName(){return presetName->stringValue();}
    String getFilter(){return filter->stringValue();}
    String getType(){String st = subType->stringValue();return type->stringValue()+(st.isEmpty()?"":String("_")+st);}
    String getOriginUID(){return originUID->stringValue();}
    ParameterContainer* getOriginContainer(){
        Uuid ui (getOriginUID());
        return ui.isNull()?nullptr:ParameterContainer::getForUidGlobal(ui);
    }

    void clear();

    void updateSubTypeName();
    DynamicObject* createObject() final;
    void configureFromObject(DynamicObject * ) final;

    DynamicObject * getPresetValueObject();
    void configurePresetValueObject(DynamicObject *v);
    bool isValidPreset();
    StringParameter* originAddress; // used as info if origin container has been deleted
private:
    StringParameter* filter; //Used to filter which preset to propose depending on the object (specific nodes, vst, controller, etc.)
    StringParameter* presetName;
    StringParameter* subType;
    StringParameter* type;
    StringParameter * originUID;
    var  values;

    WeakReference<Preset>::Master masterReference;
    friend class WeakReference<Preset>;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Preset)
};


class ParameterContainer;
class PresetSync;

class Presetable : ControllableContainer::Listener,ParameterBase::Listener{
public:
    Presetable(ParameterContainer * );
    ~Presetable();
    static const Identifier presetIdentifier;

    

    //////////////////
    //// preset


    bool loadPreset ( Preset* preset,bool sendNotif=true);
    void deletePreset(Preset * p);
    Preset* saveNewPreset (const String& name,void * notif=nullptr);
    Preset * addNamedPreset (const String& name,bool doLoad=true,void * notif=nullptr);
    const Preset * getCurrentPreset()const;
    bool hasPresetLoaded();

    bool saveCurrentPreset( void * notif=nullptr);
    int getNumPresets( );


    bool resetToCurrentPreset();
    void cleanUpPresets( );

    String getPresetFilter();
    String getType();

    ParameterBase* currentPresetName;
    ParameterBase* savePresetTrigger;


    Preset* currentPreset = nullptr;

    class Listener{
    public:
        virtual ~Listener(){}
        virtual void controllableContainerPresetLoaded (ControllableContainer*,Preset *p) =0;
    };
    ListenerList<Listener> presetableListeners;
    bool isOneOfPresetableParam(ParameterBase * );
    static DynamicObject * createPresetObject(ParameterContainer  *);
private:
    ParameterContainer * pc;

    Array<ParameterBase*> params;
    virtual void containerWillClear (ControllableContainer* ) override;

    PresetSync* presetSync = nullptr;

    void newMessage (const ParameterBase::ParamWithValue& pv)final;
    void parameterValueChanged ( ParameterBase* p, ParameterBase::Listener * notifier) final;
};
