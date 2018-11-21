/*
  ==============================================================================

    GlobalKeyListener.cpp
    Created: 21 Nov 2018 4:49:26pm
    Author:  Martin Hermant

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

