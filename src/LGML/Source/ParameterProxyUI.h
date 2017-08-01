/*
  ==============================================================================

    ParameterProxyUI.h
    Created: 31 May 2016 4:40:08pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef PARAMETERPROXYUI_H_INCLUDED
#define PARAMETERPROXYUI_H_INCLUDED

#include "ParameterProxy.h"
#include "ParameterUI.h"
#include "ControllableHelpers.h"

class ParameterProxyUI :
	public ParameterUI,
	public ParameterProxy::ParameterProxyListener,
	public ControllableReferenceUI::Listener
{
public:
	ParameterProxyUI(ParameterProxy * proxy=nullptr);
	virtual ~ParameterProxyUI();

	ControllableReferenceUI chooser;

	ParameterProxy * paramProxy;

	ScopedPointer<Component> linkedParamUI;
  

	void resized()override ;

	void setLinkedParamUI(Parameter *);



	virtual void linkedParamChanged(ParameterProxy * c) override;
	virtual void choosedControllableChanged(ControllableReferenceUI*,Controllable * c) override;

	virtual void controllableNameChanged(Controllable * c) override;

};



#endif  // PARAMETERPROXYUI_H_INCLUDED
