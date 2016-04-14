/*
  ==============================================================================

    NodeConnectionEditorLink.cpp
    Created: 29 Mar 2016 7:00:07pm
    Author:  bkupe

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "NodeConnectionEditorLink.h"

#include "Style.h"
#include "UIHelpers.h"

//==============================================================================
NodeConnectionEditorLink::NodeConnectionEditorLink()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    inSlot = nullptr;
    outSlot = nullptr;
}

NodeConnectionEditorLink::~NodeConnectionEditorLink()
{

}

void NodeConnectionEditorLink::paint (Graphics& g)
{
    g.setColour(NORMAL_COLOR);

    //Point<int> = getLocal
}

void NodeConnectionEditorLink::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}
