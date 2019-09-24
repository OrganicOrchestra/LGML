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


#ifndef PARAMETERUI_H_INCLUDED
#define PARAMETERUI_H_INCLUDED

#include "../Parameter.h"
#include "../../../JuceHeaderUI.h"
#include "../../../UI/Inspector/InspectableComponent.h"


class DefferTimer;



class ParameterUI : public InspectableComponent,
    protected ParameterBase::Listener,
    public Controllable::Listener

{
public:
    ParameterUI ( ParameterBase* parameter);
    virtual ~ParameterUI();

    WeakReference<ParameterBase> parameter;

    bool showLabel;
    bool showValue;

    void setCustomText (const String text);
    String getDisplayedText () const;

    enum MappingState
    {
        NOMAP,
        MAPSOURCE,
        MAPDEST
    };

    void setMappingState (const bool  s);
    void setMappingDest (bool _isMappingDest);
    
    bool isDraggable;

    void updateOverlayEffect();

    void visibilityChanged() final;
    void paint(Graphics & g)override;
    void parentHierarchyChanged()override;
    struct Listener{
        virtual ~Listener(){}
        virtual void displayedTextChanged(ParameterUI * ) = 0;
    };
    ListenerList<ParameterUI::Listener> paramUIListeners;

protected:

    String customTextDisplayed;
    virtual void displayedTextChangedInternal(){};
    // helper to spot wrong deletion order
    bool shouldBailOut();

    // here we are bound to only one parameter so no need to pass parameter*
    // for general behaviour see AsyncListener
    virtual void valueChanged (const var& ) {};
    virtual void rangeChanged ( ParameterBase* ) {};

    
    String getTooltip() override;
    virtual void mouseDown (const MouseEvent& e) override;
    virtual void mouseUp (const MouseEvent& e) override;

private:
    // see ParameterBase::AsyncListener
    virtual void newMessage (const ParameterBase::ParamWithValue& p) override;

    // never change this as value can be changed from other threads
    void parameterValueChanged ( ParameterBase* , ParameterBase::Listener * /*notifier=nullptr*/) override {};
    void parameterRangeChanged ( ParameterBase* )override {};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParameterUI)
    friend class LGMLDragger;
    friend class FastMapper;

    void setHasMappedParameter(bool s);
    MappingState mappingState;
    bool hasValidControllable;
    std::unique_ptr<ImageEffectFilter> mapEffect;



    // Inherited via Listener
    virtual void controllableStateChanged (Controllable* c) override;
    virtual void controllableNameChanged (Controllable*) override;
    virtual void controllableControlAddressChanged (Controllable* c) override;


    bool isMappingDest;

    var lastValuePainted;


protected:
    
    typedef HashMap<int, String> UICommandType;
    virtual void processUICommand(int cmd){}
    virtual const UICommandType & getUICommands() const;
private:
    WeakReference<ParameterUI>::Master masterReference;
    friend class WeakReference<ParameterUI>;

    bool wasShowing;
    bool hasMappedParameter;
    std::unique_ptr<DefferTimer> defferTimer;
    friend class DefferTimer;

};



//    this class allow to automaticly generate label / ui element for parameter listing in editor
//    it owns the created component
class NamedParameterUI : public ParameterUI, public Label::Listener
{
public:
    NamedParameterUI (std::unique_ptr<ParameterUI> ui, int _labelWidth, bool labelAbove = false);
    void resized()override;
    std::unique_ptr <ParameterUI > ownedParameterUI; // take care of order of init with controllablelabel
    bool labelAbove;
    void labelTextChanged (Label* labelThatHasChanged) override;
    std::unique_ptr<Label> controllableLabel;
    int labelWidth;

    void controllableControlAddressChanged (Controllable*)override;

    
};





#endif  // PARAMETERUI_H_INCLUDED
