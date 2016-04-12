/*
  ==============================================================================

    SelectableComponent.h
    Created: 26 Mar 2016 9:13:51pm
    Author:  Martin Hermant

  ==============================================================================
*/

#ifndef SELECTABLECOMPONENT_H_INCLUDED
#define SELECTABLECOMPONENT_H_INCLUDED


// base class for component that can be selected and notify listeners
// Juce handle focus but its exclusive
// ( so abstract that any component can use that as long as there is a manager to handle it)
#include "JuceHeader.h"
class SelectableComponentHandler;

class SelectableComponent : public Component {


public:
    SelectableComponent(SelectableComponentHandler * handler);
    virtual ~SelectableComponent();


    // called to change state manually
    void askForSelection(bool _isSelected, bool unique = false);

    bool isSelected;

    // can be overriden for self update
    virtual void internalSetSelected(bool) {}

    void setHandler(SelectableComponentHandler * h);
    virtual void paintOverChildren(Graphics & g)override;

protected:
//    can change this if repaint is handled in setSelected
    bool repaintOnSelection;

private:


    SelectableComponentHandler * handler;
};





#endif  // SELECTABLECOMPONENT_H_INCLUDED
