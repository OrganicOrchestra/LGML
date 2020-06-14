/*
  ==============================================================================

    ParameterUIHelpers.h
    Created: 13 Jul 2019 4:40:05pm
    Author:  Martin Hermant

  ==============================================================================
*/

#pragma once

#include "../../../UI/Style.h"
#include "ParameterUI.h"

class AllParamType{
public:
    typedef WeakReference<ParameterUI> Ptr;
    void add(ParameterUI*);
    void removeAllInstancesOf(ParameterUI*);
    typedef Array< Ptr > ArrayType;
    ArrayType getForParameter(ParameterBase *  ) const;
    static AllParamType & getAllParameterUIs(){
        static AllParamType allParameterUIs;
        return allParameterUIs;
    }
private:
    HashMap<ParameterBase*,ArrayType> container;
    HashMap<ParameterUI* , ParameterBase*> allPs;

};



///===============================


class DefferTimer : public Timer{
public:
    DefferTimer(ParameterUI * p):pui(p){}
    ~DefferTimer(){stopTimer();}
    void timerCallback()override{
        stopTimer();
        if(pui.get()){
        pui->valueChanged(v);
        }
        else{
            jassertfalse;
        }

    }
    void trigger(var value){
        v = value;
        if(!isTimerRunning())startTimer(50);
    }
    WeakReference<ParameterUI>  pui;
    var v;
};


//-------------



class LabelLinkedTooltip : public Label{
public:
    LabelLinkedTooltip(TooltipClient * p):t(p){
        Label::setJustificationType (Justification::centredLeft);
        Label::setVisible(false);
        Label::setBorderSize(BorderSize<int>(0));
        setPaintingIsUnclipped(true);
        LGMLUIUtils::optionallySetBufferedToImage(this);
    }

    String getTooltip(){ // force override weirdly works on osx
        return t->getTooltip();
    }



private:
    TooltipClient * t;

};

