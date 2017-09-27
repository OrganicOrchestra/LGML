/*
  ==============================================================================

    MainWindow.h
    Created: 11 Sep 2017 1:48:29pm
    Author:  Martin Hermant

  ==============================================================================
*/

#pragma once
#include "../JuceHeaderUI.h"
#include <juce_opengl/juce_opengl.h>
#include "LGMLDragger.h"

class MainContentComponent;
class Engine;

class MainWindow    : public DocumentWindow, private Timer
{
public:
    MainWindow (String name, Engine* e) ;
    void focusGained (FocusChangeType cause)override;


    void closeButtonPressed() override;

    void timerCallback() override;


    /* Note: Be careful if you override any DocumentWindow methods - the base
     class uses a lot of them, so by overriding you might break its functionality.
     It's best to do all your work in your content component instead, but if
     you really have to override any DocumentWindow methods, make sure your
     subclass also calls the superclass's method.
     */
    MainContentComponent* mainComponent;

#if JUCE_OPENGL
    OpenGLContext openGLContext;
#endif

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)

};
