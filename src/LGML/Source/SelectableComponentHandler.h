/*
  ==============================================================================

    SelectableComponentHandler.h
    Created: 28 Mar 2016 3:07:10pm
    Author:  Martin Hermant

  ==============================================================================
*/

#ifndef SELECTABLECOMPONENTHANDLER_H_INCLUDED
#define SELECTABLECOMPONENTHANDLER_H_INCLUDED

#include "JuceHeader.h"
class SelectableComponent;


class SelectableComponentHandler{
public:

    void internalSelected(SelectableComponent * ,bool state,bool unique);

    void removeAllSelected();

    //
    class  SelectableHandlerListener
    {
    public:
        /** Destructor. */
        virtual ~SelectableHandlerListener() {}
        virtual void selectableChanged(SelectableComponent*  c,bool isSelected) = 0;
    };

    void addSelectableHandlerListener(SelectableHandlerListener* newListener) { selectableHandlerListeners.add(newListener); }
    void removeSelectableHandlerListener(SelectableHandlerListener* listener) { selectableHandlerListeners.remove(listener); }
    ListenerList<SelectableHandlerListener> selectableHandlerListeners;

    Array<SelectableComponent*> selected;
};



#endif  // SELECTABLECOMPONENTHANDLER_H_INCLUDED
