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
    protected ParameterBase::Listener,
    public Controllable::Listener

{
public:
    ParameterUI ( ParameterBase* parameter);
    virtual ~ParameterUI();

    WeakReference<ParameterBase> parameter;

    bool showLabel;
    bool showValue;
    static const Array<WeakReference<ParameterUI>> & getAllParameterUIs();
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

    void updateOverlayEffect();

    void visibilityChanged() final;
    void parentHierarchyChanged()final;

protected:

    String customTextDisplayed;
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
    ScopedPointer<ImageEffectFilter> mapEffect;



    // Inherited via Listener
    virtual void controllableStateChanged (Controllable* c) override;
    virtual void controllableControlAddressChanged (Controllable* c) override;


    bool isMappingDest;

    var lastValuePainted;

protected:
    typedef HashMap<int, String> UICommandType;
    virtual void processUICommand(int cmd){};
    virtual const UICommandType & getUICommands() const;
private:
    WeakReference<ParameterUI>::Master masterReference;
    friend class WeakReference<ParameterUI>;

    bool wasShowing;
    bool hasMappedParameter;


};



class LabelLinkedTooltip : public Component,public TooltipClient{
public:
    LabelLinkedTooltip(TooltipClient * p):t(p){
        label.setJustificationType (Justification::centredLeft);
        addAndMakeVisible(label);
        label.setVisible(false);
        label.setBorderSize(BorderSize<int>(0));
    }
        void paint(Graphics & g) override{
            getLookAndFeel().drawLabel (g, label);
        }
    void resized()override{
        label.setBounds(getLocalBounds());
    }
    String getTooltip() override{
        return t->getTooltip();
    }
    void addListener(Label::Listener *l){
        label.addListener(l);
    }
    void setEditable(bool editOnSingleClick,
                     bool editOnDoubleClick=false,
                     bool lossOfFocusDiscards=false){
        label.setEditable(editOnSingleClick,editOnDoubleClick,lossOfFocusDiscards);
    }

    void setFont(const Font & f){label.setFont(f);}
    Font getFont(){return label.getFont();}
    String getText(){return label.getText();}
    void setText(const String & s,juce::NotificationType notif){label.setText(s,notif);}

private:
    Label label;
    TooltipClient * t;
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
    ScopedPointer<LabelLinkedTooltip> controllableLabel;
    int labelWidth;
    ScopedPointer <ParameterUI > ownedParameterUI;
    void controllableControlAddressChanged (Controllable*)override;

    
};





#endif  // PARAMETERUI_H_INCLUDED
