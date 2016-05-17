/*
  ==============================================================================

    JsLiveEditor.cpp
    Created: 16 May 2016 4:58:35pm
    Author:  bkupe

  ==============================================================================
*/

#include "JsLiveEditor.h"

CPlusPlusCodeTokeniser  JsLiveEditor::codeTokeniser;

JsLiveEditor::JsLiveEditor(CodeDocument * document) :
	CodeEditorComponent(*document,&codeTokeniser)
{
	
}

JsLiveEditor::~JsLiveEditor()
{

}
