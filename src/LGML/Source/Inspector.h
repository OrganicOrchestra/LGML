/*
  ==============================================================================

    Inspector.h
    Created: 9 May 2016 6:41:38pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef INSPECTOR_H_INCLUDED
#define INSPECTOR_H_INCLUDED

#include "ShapeShifterContent.h"
#include "InspectableComponent.h"
#include "InspectorEditor.h"

class Inspector : public ShapeShifterContent
{
public:
	Inspector();
	virtual ~Inspector();

	InspectableComponent * currentComponent;

	ScopedPointer<InspectorEditor> currentEditor;

	void setCurrentComponent(InspectableComponent * component);

	void resized() override;

	void clear();
	void inspectCurrentComponent();
};


#endif  // INSPECTOR_H_INCLUDED
