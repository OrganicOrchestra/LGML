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

class JsNodeUI : public NodeBaseContentUI,public ControllableContainerListener,JsEnvironment::Listener{


public:
    JsNodeUI();
    ~JsNodeUI();


    void init()override;

    void resized() override;

    void newJsFileLoaded(bool )override;
    ScopedPointer<JsEnvironmentUI> jsUI;

    OwnedArray<ControllableUI> varUI;
    void layoutUI();

    void controllableAdded(Controllable *) override;
    void controllableRemoved(Controllable *) override;


};



#endif  // JSNODEEDITOR_H_INCLUDED
