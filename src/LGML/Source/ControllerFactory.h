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
#include "JavaScriptController.h"
#include "OSCCustomController.h"


class ControllerManager;

static const String controllerTypeNames[] = { "OSC Direct","ScriptedOSC","OSC Custom","DMX","MIDI" };

class ControllerFactory
{
public:
    enum ControllerType
    {
        OSCDirect,
        ScriptedOSC,
		OSCCustom,
        DMX,
        MIDI,
        UNKNOWN //has to be last
    };

    ControllerFactory()
    {

    }

    ~ControllerFactory()
    {

    }

    Controller * createController(ControllerType controllerType)
    {
        Controller * c = nullptr;

        switch (controllerType)
        {
            case OSCDirect:
                c = new OSCDirectController();
                break;
            case ScriptedOSC:
                c = new JavascriptController();
                break;
			case OSCCustom:
				c = new OSCCustomController();
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


        c->controllerTypeEnum = (int)controllerType;
        return c;
    }

    static PopupMenu * getControllerTypesMenu(int menuIdOffset = 0)
    {
        PopupMenu * p = new PopupMenu();
        for (int i = 0; i < numElementsInArray(controllerTypeNames); i++)
        {
            p->addItem(menuIdOffset + i + 1, controllerTypeNames[i]);
        }

        return p;
    }

    static ControllerType getTypeFromString(const String & s) {
        for (int i = 0; i < numElementsInArray(controllerTypeNames); i++)
        {
            if (s == controllerTypeNames[i]) { return ControllerType(i); }
        }
        return UNKNOWN;
    }

    static String controllerTypeToString(ControllerType t) {
        if (t<0 || t > UNKNOWN)return String::empty;
        return controllerTypeNames[(int)t];
    }

    static String controllerToString(Controller *  c) {
        int t = c->controllerTypeEnum;
        if (t<0 || t > UNKNOWN)return String::empty;
        return controllerTypeNames[(int)t];
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ControllerFactory)
};



#endif  // CONTROLLERFACTOR_H_INCLUDED
