/*
  ==============================================================================

    GlobalKeyListener.h
    Created: 21 Nov 2018 4:49:26pm
    Author:  Martin Hermant

  ==============================================================================
*/

#pragma once

#include "../JuceHeaderUI.h"
class GlobalKeyListener : public KeyListener{
public:
    static void addTraversingListener(KeyListener *k);
    static void removeTraversingListener(KeyListener *k);
private:
    static GlobalKeyListener * i();
    static GlobalKeyListener * instance;

    Array<KeyListener*> keyListeners;
    bool keyPressed (const KeyPress& key,
                     Component* originatingComponent) override;
    bool keyStateChanged (bool isKeyDown, Component* originatingComponent) override;
};
