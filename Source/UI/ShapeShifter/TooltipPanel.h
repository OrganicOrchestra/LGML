/*
  ==============================================================================

    TooltipShifter.h
    Created: 3 Nov 2017 10:44:48am
    Author:  Martin Hermant

  ==============================================================================
*/

#pragma once

class TooltipPanel :
public ShapeShifterContentComponent,
private Timer{
public:
    TooltipPanel(const String & n):ShapeShifterContentComponent(n),comp(nullptr){
        addAndMakeVisible(label);
        startTimer(50);
        label.setReadOnly(true);
        label.setMultiLine(true);
        label.setColour(TextEditor::ColourIds::backgroundColourId, findColour(Label::ColourIds::backgroundColourId));


    }
     void timerCallback() override{
         auto& desktop = Desktop::getInstance();
         auto mouseSource = desktop.getMainMouseSource();
         auto* newComp = mouseSource.isTouch() ? nullptr : mouseSource.getComponentUnderMouse();

         if( newComp!=comp.get() && newComp!=&label){
             TooltipClient* tc = dynamic_cast<TooltipClient*>(newComp);
             if(!tc) tc = findParentComponentOfClass<TooltipClient>();
            if(tc) updateLabel(tc);
             comp = newComp;
         }
    }

    void updateLabel(TooltipClient* tc){
        String newTT = tc->getTooltip();
        if(newTT.isNotEmpty()){
            label.setText(newTT, dontSendNotification);
        }
        else{
//            jassertfalse;
        }
    }
    void resized()override{
        label.setBounds(getLocalBounds());
    }

    WeakReference<Component> comp;
    TextEditor label;
};
