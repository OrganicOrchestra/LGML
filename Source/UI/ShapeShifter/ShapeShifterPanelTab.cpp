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

#include "ShapeShifterPanelTab.h"
#include "../Style.h"
#include "ShapeShifterManager.h"


ShapeShifterPanelTab::ShapeShifterPanelTab (ShapeShifterContent* _content) : content (_content), selected (false)
{
    panelLabel.setInterceptsMouseClicks (false, false);

    panelLabel.setFont (12);
    panelLabel.setJustificationType (Justification::centred);
    panelLabel.setText (juce::translate(content == nullptr ? "[No content]" : content->contentName), NotificationType::dontSendNotification);

    addAndMakeVisible (&panelLabel);

    Image removeImage = ImageCache::getFromMemory (BinaryData::removeBT_png, BinaryData::removeBT_pngSize);

    closePanelBT.setImages (false, true, true, removeImage,
                            0.7f, Colours::transparentBlack,
                            removeImage, 1.0f, Colours::transparentBlack,
                            removeImage, 1.0f, Colours::white.withAlpha (.7f),
                            0.5f);
    closePanelBT.addListener (this);

    addAndMakeVisible (closePanelBT);


    setSize (getLabelWidth(), 20);
    setOpaque (true);
    setTooltip(content->info);


}

ShapeShifterPanelTab::~ShapeShifterPanelTab()
{
    
}

void ShapeShifterPanelTab::setSelected (bool value)
{
    selected = value;
    repaint();
}

void ShapeShifterPanelTab::paint (Graphics& g)
{
    if(isTimerRunning()){
        auto pct =  (Time::getCurrentTime() - blinkStartTime).inMilliseconds()*1.0/notificationDurationMs;
        g.setColour(blinkColour.interpolatedWith(Colours::white, (1+sin(pct * MathConstants<float>::twoPi * 2))/2));

    }
    else{
    g.setColour (selected ? findColour (ResizableWindow::backgroundColourId) : findColour (ResizableWindow::backgroundColourId).brighter (.15f));
    }
    Rectangle<int> r = getLocalBounds();

    //  if (!selected) r.reduce(1,1);
    g.fillRect (r);
}

void ShapeShifterPanelTab::resized()
{
    Rectangle<int> r = getLocalBounds();
    closePanelBT.setBounds (r.removeFromRight (r.getHeight()).reduced (3));
    panelLabel.setBounds (r);
}

int ShapeShifterPanelTab::getLabelWidth()
{
    return panelLabel.getFont().getStringWidth (panelLabel.getText()) + 30;
}

void ShapeShifterPanelTab::buttonClicked (Button* b)
{
    if (b == &closePanelBT) tabListeners.call (&TabListener::askForRemoveTab, this);
}

void ShapeShifterPanelTab::blink(const Colour & c) {
    startTimer(30);
    blinkColour = c;
    blinkStartTime = Time::getCurrentTime();

}

void ShapeShifterPanelTab::timerCallback() {
    if((Time::getCurrentTime() - blinkStartTime).inMilliseconds() > notificationDurationMs){
        stopTimer();
    };
    repaint();
    
}

#endif
