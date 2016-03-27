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


class SelectableComponent : public Component{


public:
    SelectableComponent():isSelected (false),repaintOnSelection(true){}
    virtual ~SelectableComponent(){}


    // called by handler
    void internalSetSelected(bool _isSelected) {
        isSelected = _isSelected;
        setSelected(isSelected);
        listeners.call(&SelectableComponent::Listener::selectableSelected,this,isSelected);
        if(repaintOnSelection)repaint();
    };

    // overriden if needed
    virtual void setSelected(bool isSelected) {};

    bool isSelected;



    class  Listener
    {
    public:
        /** Destructor. */
        virtual ~Listener() {}
        virtual void selectableSelected(SelectableComponent * c,bool state) = 0;
    };

    void addSelectableListener(SelectableComponent::Listener* newListener) { listeners.add(newListener); }
    void removeSelectableListener(SelectableComponent::Listener* listener) { listeners.remove(listener); }

protected:
//    can change this if repaint is handled in setSelected
    bool repaintOnSelection;

private:
    ListenerList<SelectableComponent::Listener> listeners;


};


#endif  // SELECTABLECOMPONENT_H_INCLUDED
