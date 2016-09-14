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
	public ControllableChooser::Listener,
	public ButtonListener
{
public:
	ParameterProxyUI(ParameterProxy * proxy);
	virtual ~ParameterProxyUI();

	ControllableChooser chooser;

	ParameterProxy * paramProxy;
	ScopedPointer<StringParameterUI> aliasParam;
	ScopedPointer<ParameterUI> linkedParamUI;
	ImageButton removeBT;

	void resized()override ;

	void setLinkedParamUI(Parameter *);

	void buttonClicked(Button * b)override;

	virtual void linkedParamChanged(Parameter * c) override;
	virtual void choosedControllableChanged(Controllable * c) override;

	virtual void controllableNameChanged(Controllable * c) override;

};



#endif  // PARAMETERPROXYUI_H_INCLUDED
