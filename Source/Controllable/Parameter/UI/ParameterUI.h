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


class ParameterUI : public InspectableComponent,
    protected Parameter::AsyncListener,
    private Parameter::Listener,
    public SettableTooltipClient,
    public Controllable::Listener
{
public:
    ParameterUI (Parameter* parameter);
    virtual ~ParameterUI();

    WeakReference<Parameter> parameter;

    bool showLabel;
    bool showValue;

    void setCustomText (const String text);

    enum MappingState
    {
        NOMAP,
        MAPSOURCE,
        MAPDEST
    };

    void setMappingState (const bool  s);
    void setMappingDest (bool _isMappingDest);

    bool isDraggable;
    bool isSelected;



protected:

    String customTextDisplayed;
    // helper to spot wrong deletion order
    bool shouldBailOut();

    // here we are bound to only one parameter so no need to pass parameter*
    // for general behaviour see AsyncListener
    virtual void valueChanged (const var& ) {};
    virtual void rangeChanged (Parameter* ) {};

    void updateTooltip();
    virtual void mouseDown (const MouseEvent& e) override;


private:
    // see Parameter::AsyncListener
    virtual void newMessage (const Parameter::ParamWithValue& p) override;

    // never change this as value can be changed from other threads
    void parameterValueChanged (Parameter* ) override {};
    void parameterRangeChanged (Parameter* )override {};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParameterUI)
    friend class LGMLDragger;
    MappingState mappingState;
    bool hasValidControllable;
    ScopedPointer<ImageEffectFilter> mapEffect;



    // Inherited via Listener
    virtual void controllableStateChanged (Controllable* c) override;
    virtual void controllableControlAddressChanged (Controllable* c) override;


    bool isMappingDest;

    WeakReference<ParameterUI>::Master masterReference;
    friend class WeakReference<ParameterUI>;




};


//    this class allow to automaticly generate label / ui element for parameter listing in editor
//    it owns the created component
class NamedParameterUI : public ParameterUI, public Label::Listener
{
public:
    NamedParameterUI (ParameterUI* ui, int _labelWidth, bool labelAbove = false);
    void resized()override;
    bool labelAbove;
    void labelTextChanged (Label* labelThatHasChanged) override;
    Label controllableLabel;
    int labelWidth;
    ScopedPointer <ParameterUI > ownedParameterUI;
};





#endif  // PARAMETERUI_H_INCLUDED
