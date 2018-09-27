/*
 ==============================================================================

 Copyright © Organic Orchestra, 2017

 This file is part of LGML. LGML is a software to manipulate sound in real-time

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation (version 3 of the License).

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 ==============================================================================
 */

#if !ENGINE_HEADLESS
#include "ProgressWindow.h"
#include "Style.h"

ProgressWindow::ProgressWindow (const String& _title, ProgressNotifier* notifier) :
    progressParam ("Progress", "Progression", 0, 0, 1),
    titleLabel ("title", ""),
    ProgressListener (notifier)
{
    DBG ("progressParam " << progressParam.floatValue());

    progressUI = new FloatSliderUI (&progressParam);
    addAndMakeVisible (progressUI);
    addAndMakeVisible (titleLabel);
    titleLabel.setText (_title, dontSendNotification);
}

ProgressWindow::~ProgressWindow()
{
}

void ProgressWindow::paint (Graphics& g)
{
    g.fillAll (Colours::black.withAlpha (.5f));

    Rectangle<int> r = getLocalBounds().withSizeKeepingCentre (windowWidth, windowHeight);
    g.setColour (findColour (ResizableWindow::backgroundColourId));
    g.fillRoundedRectangle (r.toFloat(), 2);
    g.setColour (findColour (ResizableWindow::backgroundColourId).brighter (.2f));
    g.drawRoundedRectangle (r.toFloat(), 2, 1);
}

void ProgressWindow::resized()
{
    Rectangle<int> r = getLocalBounds().withSizeKeepingCentre (windowWidth, windowHeight).reduced (5);
    titleLabel.setBounds (r.removeFromTop (15));
    r.removeFromTop (20);
    progressUI->setBounds (r.removeFromTop (20));
}
void ProgressWindow::startedProgress (ProgressTask* task)
{
    titleLabel.setText (task->getAddress().joinIntoString (" / "), NotificationType::dontSendNotification);
}
void ProgressWindow::endedProgress (ProgressTask* /*task*/)
{

}

void ProgressWindow::newProgress (ProgressTask* task, float /*advance*/)
{
    setProgress (task->getNormalizedProgress());
};
void ProgressWindow::setProgress (float progress)
{
    progressParam.setValue (progress);
}
#endif
