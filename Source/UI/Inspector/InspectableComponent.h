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


#ifndef INSPECTABLECOMPONENT_H_INCLUDED
#define INSPECTABLECOMPONENT_H_INCLUDED

#include "../../JuceHeaderUI.h"//keep

class ParameterContainer;
class ParameterBase;
class Inspector;
class InspectorEditor;


class InspectableComponent : public juce::Component,public TooltipClient
{
public:
    explicit InspectableComponent (ParameterContainer* relatedContainer , const String& inspectableType = "none");
    explicit InspectableComponent ( ParameterBase* relatedParameter , const String& inspectableType = "none");
    explicit InspectableComponent (const String& inspectableType );

    virtual ~InspectableComponent();

    Inspector* inspector;
    const String inspectableType;

    ParameterContainer* getRelatedParameterContainer();
    ParameterBase * getRelatedParameter();
    virtual InspectorEditor* createEditor();


    int recursiveInspectionLevel;
    bool canInspectChildContainersBeyondRecursion;

    bool paintBordersWhenSelected;
    bool bringToFrontOnSelect;

    bool isSelected;

    void selectThis();

    String getTooltip() override ;
    virtual DynamicObject * createObject();
    
protected:
    bool keyPressed (const KeyPress&)override;

    void paintOverChildren (juce::Graphics& g) override;


    virtual void setSelectedInternal (bool value); //to be overriden
    void setVisuallySelected (bool value);
    friend class Inspector;
    void mouseUp (const MouseEvent&) override;
    WeakReference<ParameterContainer> relatedParameterContainer;
    WeakReference<ParameterBase> relatedParameter;
private:
    WeakReference<InspectableComponent>::Master masterReference;
    friend class WeakReference<InspectableComponent>;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InspectableComponent)
};



#endif  // INSPECTABLECOMPONENT_H_INCLUDED
