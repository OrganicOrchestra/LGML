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

#include "ShapeShifterContent.h"

ShapeShifterContent::ShapeShifterContent (Component* _contentComponent, const String& _contentName,const String& _info) :
    contentIsFlexible (false),
    contentName (_contentName),
    contentIsShown (false),
    contentComponent (_contentComponent),
    info(juce::translate(_info))
{

}

ShapeShifterContent::~ShapeShifterContent()
{
}


ShapeShifterContentComponent::ShapeShifterContentComponent (const String& contentName,const String & _info) :
    ShapeShifterContent (this, contentName,_info),
    infoLabel("info",juce::translate(_info))
{
    addAndMakeVisible(infoLabel);
    infoLabel.setAlpha(0.5);
    infoLabel.toBack();
    infoLabel.setJustificationType(Justification::centred);
}
void ShapeShifterContentComponent::resized(){
    infoLabel.setBounds(getLocalBounds());
}


#endif
