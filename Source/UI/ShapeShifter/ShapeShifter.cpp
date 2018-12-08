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

#if !ENGINE_HEADLESS

#include "ShapeShifter.h"
#include "ShapeShifterContainer.h"


const int ShapeShifter::minSize(50);

ShapeShifter::ShapeShifter (Type _type) :
    shifterType (_type),
    preferredWidth (300), preferredHeight (300),
    parentShifterContainer (nullptr),
    isMini( false)
{

addMouseListener(this, true);
}

ShapeShifter::~ShapeShifter()
{ masterReference.clear();
}

void ShapeShifter::setPreferredWidth (int newWidth)
{
    preferredWidth = jmax<int> (minSize, newWidth);
}

void ShapeShifter::setPreferredHeight (int newHeight)
{
    preferredHeight = jmax<int> (minSize, newHeight);

}



int ShapeShifter::getPreferredWidth()
{
    jassert(preferredWidth>0);
    return preferredWidth;
}

int ShapeShifter::getPreferredHeight()
{
    jassert(preferredHeight>0);
    return preferredHeight;
}

int ShapeShifter::getMinWidth(){
    return minSize;
}

int ShapeShifter::getMinHeight(){
    return minSize;
}
bool ShapeShifter::isDetached()
{
    return parentShifterContainer == nullptr;
}

void ShapeShifter::setParentContainer (ShapeShifterContainer* _parent)
{
    if (_parent == parentShifterContainer) return;

    parentShifterContainer = _parent;
}


var ShapeShifter::getCurrentLayout()
{

    auto * dob = new DynamicObject();
    dob->setProperty ("type", (int)shifterType);
    dob->setProperty ("width", preferredWidth);
    dob->setProperty ("height", preferredHeight);
    dob->setProperty("isMini",isMini || (miniTimer!=nullptr));
    var layout (dob);
    return layout;
}

void ShapeShifter::loadLayout (var layout)
{
    auto dob = layout.getDynamicObject();
    setPreferredWidth (dob->getProperty ("width"));
    setPreferredHeight (dob->getProperty ("height"));
    setMini(dob->getProperty("isMini"),false);
    loadLayoutInternal (layout);
    resized();
}


void ShapeShifter::mouseUp(const MouseEvent & me){
    if(isMini){
        setMini(false);
    }

}
class MiniTimer : private MultiTimer{
public:
    MiniTimer(ShapeShifter * _s):s(_s){
        startTimer(1,200);
    }
    void requestEnd(){
        stopTimer(1);
        startTimer(2,300);
    }
    bool isRunning(){
        return isTimerRunning(1) || isTimerRunning(2);
    }

    void stopTimers(){
        stopTimer(1) ;
        stopTimer(2);
    }
    void setMini(){
        for(auto o:s->parentShifterContainer->shifters){
            if(o->miniTimer && o->miniTimer!=this){
                o->setMini(true,false);
                o->miniTimer = nullptr;
            }
        }
        s->setMini(false,false);
        s->parentShifterContainer->resized();
    }
    void timerCallback(int id) override{
        if(s.get()){
            if(id==1){
                setMini();
                stopTimer(1);
            }
            if(id==2){
                stopTimer(2);
                s->setMini(true);
                s->miniTimer = nullptr;
            }
        }
        else{
            stopTimer(1);
            stopTimer(2);
        }
    }

    WeakReference<ShapeShifter> s;
};

void ShapeShifter::setMini(bool s,bool resizeNow){
//    DBG(String(s?"":"not")+" mini : "+ getName());

    if(miniTimer){
        if(!s && !miniTimer->isRunning()){miniTimer=nullptr;} // be sure to delete it if stopped
    }
    isMini = s;
    ShapeShifterContainer * parent = parentShifterContainer;
    if(parent){
        bool allMinimized = true;
        for(auto & s:parent->shifters){allMinimized&=s->isMini;}
        if (resizeNow && allMinimized){jassert(!isMini);return;}

        if(auto * sp = dynamic_cast<ShapeShifterPanel*>(this)){
            if(sp->currentContent && sp->currentContent->contentComponent){
                sp->currentContent->contentComponent->setVisible(!isMini);
            }
        }
        else{
            for(auto & c:getChildren()){ // we keep this component visible to recieve mouse events
                c->setVisible(!isMini);
            }
        }

        if(resizeNow)parent->resized();
        
    }
}

void ShapeShifter::mouseEnter(const juce::MouseEvent &me){
    if(miniTimer){miniTimer->stopTimers();}
    if(isMini){

        miniTimer = new MiniTimer(this);
    }

}

void ShapeShifter::mouseExit(const MouseEvent & me){
    if(miniTimer){
        miniTimer->requestEnd();
    }
}

#endif
