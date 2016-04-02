/*
  ==============================================================================

    NodeConnectionEditor.cpp
    Created: 29 Mar 2016 6:43:49pm
    Author:  bkupe

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "NodeConnectionEditor.h"

#include "Style.h"

juce_ImplementSingleton(NodeConnectionEditor);


//==============================================================================
NodeConnectionEditor::NodeConnectionEditor() : DocumentWindow("Connection Editor",BG_COLOR,DocumentWindow::closeButton,false)
{
	currentConnection = nullptr;
	setSize(300,500);
	setResizable(true, false);
	setUsingNativeTitleBar(true);
	mainContainer.setBounds(0, 0, 300, 500);
	setContentOwned(&mainContainer,true);

}

NodeConnectionEditor::~NodeConnectionEditor()
{
	setCurrentConnection(nullptr);
	setVisible(false);
	removeFromDesktop();
}

void NodeConnectionEditor::setCurrentConnection(NodeConnection * _connection)
{
	if (currentConnection == currentConnection) return;

	if (_connection != nullptr)
	{
		clearContent();
	}

	currentConnection = _connection;

	if (currentConnection != nullptr)
	{
		generateContent();
	}
}

void NodeConnectionEditor::editConnection(NodeConnection * _connection)
{
	addToDesktop();
	setTopLeftPosition(200, 200);
	setVisible(true);
	setCurrentConnection(_connection);
}

void NodeConnectionEditor::resized()
{
	DBG("editor resized");
}

void NodeConnectionEditor::closeButtonPressed()
{
	setCurrentConnection(nullptr);
}

void NodeConnectionEditor::clearContent()
{
	inputsContainer.removeAllChildren();
	outputsContainer.removeAllChildren();
	linksContainer.removeAllChildren();
}

void NodeConnectionEditor::generateContent()
{

	resized();
}
