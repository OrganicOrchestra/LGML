/*
  ==============================================================================

    JsLiveEditor.h
    Created: 16 May 2016 4:58:35pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef JSLIVEEDITOR_H_INCLUDED
#define JSLIVEEDITOR_H_INCLUDED

#include "JuceHeader.h"//keep

class JsLiveEditor :
	public CodeEditorComponent
{
public:
	JsLiveEditor(CodeDocument * document);
	virtual ~JsLiveEditor();

	static CPlusPlusCodeTokeniser codeTokeniser;

};


#endif  // JSLIVEEDITOR_H_INCLUDED
