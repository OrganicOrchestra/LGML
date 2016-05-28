/*
  ==============================================================================

    JsNodeEditor.h
    Created: 28 May 2016 2:18:27pm
    Author:  Martin Hermant

  ==============================================================================
*/

#ifndef JSNODEEDITOR_H_INCLUDED
#define JSNODEEDITOR_H_INCLUDED

#include "JsEnvironmentUI.h"
#include "NodeBaseUI.h"

class JsNodeUI : public NodeBaseContentUI{


public:
    JsNodeUI(){
        
    };
    

    void init()override;

    void resized() override{
        jsUI->setBounds(getLocalBounds());

    }
    ScopedPointer<JsEnvironmentUI> jsUI;
    
    
};



#endif  // JSNODEEDITOR_H_INCLUDED
