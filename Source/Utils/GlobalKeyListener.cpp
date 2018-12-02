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

#include "GlobalKeyListener.h"

GlobalKeyListener * GlobalKeyListener::instance(nullptr);

void GlobalKeyListener::addTraversingListener(KeyListener *k){
    i()->keyListeners.add(k);
}
void GlobalKeyListener::removeTraversingListener(KeyListener *k){
    i()->keyListeners.removeAllInstancesOf(k);
}

GlobalKeyListener * GlobalKeyListener::i(){
    if(!instance){
        instance = new GlobalKeyListener();
    }

    if(ComponentPeer::getNumPeers()>0){
        Component * mainComp =&ComponentPeer::getPeer(0)->getComponent();
        mainComp->addKeyListener(instance);
    }

        return instance;
}


bool GlobalKeyListener::keyPressed (const KeyPress& key,
                  Component* originatingComponent) {
    bool res = false;
    for(auto k:keyListeners){
        res |=k->keyPressed(key, originatingComponent);
    }
    return res;
 }
bool GlobalKeyListener::keyStateChanged (bool isKeyDown, Component* originatingComponent) {
    bool res = false;
    for(auto k:keyListeners){
        res |=k->keyStateChanged(isKeyDown, originatingComponent);
    }
    return res;
};

