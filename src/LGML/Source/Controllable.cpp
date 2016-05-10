/*
  ==============================================================================

    Controllable.cpp
    Created: 8 Mar 2016 1:08:56pm
    Author:  bkupe

  ==============================================================================
*/

#include "Controllable.h"
#include "ControllableContainer.h"

Controllable::Controllable(const Type &type, const String & niceName, const String &description, bool enabled) :
    type(type),
    description(description),
    parentContainer(nullptr),
    hasCustomShortName(false),
    isControllableExposed(true),
    isControllableFeedbackOnly(false),
    hideInEditor(false),
	replaceSlashesInShortName(true)
{
    setEnabled(enabled);
    setNiceName(niceName);
}

String Controllable::getControlAddress(ControllableContainer * relativeTo)
{
    StringArray addressArray;
    addressArray.add(shortName);

    ControllableContainer * pc = parentContainer;

    while (pc != relativeTo)
    {
        if(!pc->skipControllableNameInAddress) addressArray.insert(0, pc->shortName);
        pc = pc->parentContainer;
    }

    return "/" + addressArray.joinIntoString("/");
}
