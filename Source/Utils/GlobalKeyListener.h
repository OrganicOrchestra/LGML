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
    friend class PluginWindow;
};
