/*
  ==============================================================================

    ControllableEditor.h
    Created: 26 Mar 2016 10:13:46pm
    Author:  Martin Hermant

  ==============================================================================
*/

#ifndef CONTROLLABLEEDITOR_H_INCLUDED
#define CONTROLLABLEEDITOR_H_INCLUDED


#include "JuceHeader.h"

class ControllableUI;
class Controllable;
class ControllableContainer;

//  base class for displaying an UI presenting all control available in ControllableContainer

class ControllableContainerEditor:public Component{
public:
    ControllableContainerEditor(ControllableContainer * ,Component*);
    virtual ~ControllableContainerEditor(){deleteAllChildren();}

    void addControlUI(ControllableUI * c);
    void removeControlUI(ControllableUI * c);
    void removeControllableFromEditor(Controllable * c);
    void removeContainerFromEditor(ControllableContainer * toRemove);
    void paint(Graphics &g) override;
    void childBoundsChanged(Component *)override;
    void resized()override;
    void syncUIElements();
    ControllableContainer * owner;
    Array<ControllableUI*> controllableUIs;
    Array<ControllableContainerEditor*> editors;
protected:
    Component* embeddedComp;


};



#endif  // CONTROLLABLEEDITOR_H_INCLUDED
