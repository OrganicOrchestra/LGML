/*
  ==============================================================================

    JsNode.h
    Created: 28 May 2016 2:00:46pm
    Author:  Martin Hermant

  ==============================================================================
*/

#ifndef JSNODE_H_INCLUDED
#define JSNODE_H_INCLUDED
#include "JsEnvironment.h"
#include "NodeBase.h"
#include "JsHelpers.h"



class JsNode : public NodeBase,public JsEnvironment{
    public :
		JsNode();
    StringParameter* scriptPath;

	void clearNamespace() override;

	void buildLocalEnv() override;

	
	void onContainerParameterChanged(Parameter * p) override;


	static var addIntParameter(const var::NativeFunctionArgs & a);
	static var addFloatParameter(const var::NativeFunctionArgs & a);
	static var addStringParameter(const var::NativeFunctionArgs & a);
	static var addBoolParameter(const var::NativeFunctionArgs & a);
	static var addTriggerParameter(const var::NativeFunctionArgs & a);

    virtual ConnectableNodeUI * createUI() override;

    Array<Controllable * > jsParameters;


};



#endif  // JSNODE_H_INCLUDED
