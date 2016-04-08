/*
  ==============================================================================

    ControllerFactor.h
    Created: 8 Mar 2016 10:25:48pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef CONTROLLERFACTOR_H_INCLUDED
#define CONTROLLERFACTOR_H_INCLUDED


#include "OSCDirectController.h"
#include "DMXController.h"
#include "MIDIController.h"

class ControllerManager;

class ControllerFactory
{
public:
    enum ControllerType
    {
        OSCDirect,
        DMX,
        MIDI
    };

    ControllerFactory()
    {

    }

    ~ControllerFactory()
    {

    }

    Controller * createController(ControllerType nodeType)
    {
        Controller * c = nullptr;

        switch (nodeType)
        {
        case OSCDirect:
            c = new OSCDirectController();
            break;

        case DMX:
            c = new DMXController();
            break;

        case MIDI:
            c = new MIDIController();
            break;

        default:
            jassert(false);
            break;
        }


        return c;
    }

    static PopupMenu * getControllerTypesMenu(int menuIdOffset = 0)
    {
        PopupMenu * p = new PopupMenu();
        static const String controllerTypeNames [] = { "OSC Direct","DMX","MIDI" };
        for (int i = 0; i < numElementsInArray(controllerTypeNames); i++)
        {
            p->addItem(menuIdOffset + i + 1, controllerTypeNames[i]);
        }

        return p;
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ControllerFactory)
};



#endif  // CONTROLLERFACTOR_H_INCLUDED
