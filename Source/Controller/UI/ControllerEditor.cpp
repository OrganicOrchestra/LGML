/* Copyright © Organic Orchestra, 2017
*
* This file is part of LGML.  LGML is a software to manipulate sound in realtime
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation (version 3 of the License).
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
*/

#if !ENGINE_HEADLESS

#include "ControllerEditor.h"
#include "../../UI/Style.h"

#include "../../Controllable/Parameter/UI/ParameterUIFactory.h"
ControllerEditor::ControllerEditor (Controller* _controller, bool generateAuto) :
    InspectorEditor(),
    controller (_controller),
    addParameterBT ("Add Variable")
{
    LGMLUIUtils::optionallySetBufferedToImage(&addParameterBT);
    addParameterBT.addListener (this);

    if (generateAuto)
    {
        editor = std::make_unique<GenericParameterContainerEditor> (controller);
        addAndMakeVisible (editor.get());
    }

    addAndMakeVisible (addParameterBT);
}

ControllerEditor::~ControllerEditor()
{

}


void ControllerEditor::resized()
{
    InspectorEditor::resized();
    auto r = getLocalBounds();
    addParameterBT.setBounds (r.removeFromTop (20));

    if (editor)
    {
        editor->setBounds (r);
    }
}

int ControllerEditor::getContentHeight() const
{
    return  20  + (editor ? editor->getContentHeight() : 0);
}


void ControllerEditor::buttonClicked (Button* b)
{
    if (b == &addParameterBT)
    {

        controller->userContainer.addNewParameter<FloatParameter> ("variable", "Custom Variable");

    }
}
#endif
