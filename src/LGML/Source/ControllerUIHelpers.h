/*
  ==============================================================================

    ControllerUIHelpers.h
    Created: 13 May 2016 12:02:49pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef CONTROLLERUIHELPERS_H_INCLUDED
#define CONTROLLERUIHELPERS_H_INCLUDED

#include "JuceHeader.h"
#include "ControlManager.h"

class ControlVariableChooser : public PopupMenu
{
public :
	ControlVariableChooser();
	virtual ~ControlVariableChooser();

	const int maxVariablesPerController = 1000; //should be enough

	ControlVariable * showAndGetVariable();
};


#endif  // CONTROLLERUIHELPERS_H_INCLUDED
