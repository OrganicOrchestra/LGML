/*
  ==============================================================================

    SpatNode.h
    Created: 2 Mar 2016 8:37:48pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef SPATNODE_H_INCLUDED
#define SPATNODE_H_INCLUDED




#include "NodeBase.h"

class SpatNode : public NodeBase
{
public:
	SpatNode();
	
	const Identifier Beam2DId = "Beam2D";
	const Identifier Beam3DId = "Beam3D";
	const Identifier Proxy2DId = "Proxy2D";
	const Identifier Proxy3DId = "Proxy3D";

	EnumParameter * spatMode; //2D Beam, 2D Proxy, 3D Beam, 3D Proxy
	IntParameter * numSpatInputs;
	IntParameter * numSpatOutputs;
	Array<FloatParameter *> targetsX;
	Array<FloatParameter *> targetsY;

	void setSourcePosition(int index, const Point<float> &position);
	void setTargetPosition(int index, const Point<float> &position);
	void updateInputOutputDataSlots();

	bool modeIs2D();
	bool modeIsBeam();

	void onContainerParameterChanged(Parameter *) override;

	//Listener
	class  SpatNodeListener
	{
	public:

		/** Destructor. */
		virtual ~SpatNodeListener() {}
		virtual void modeChanged() = 0;
		virtual void numSpatInputsChanged() = 0;
		virtual void numSpatOutputsChanged() = 0;
	};

	ListenerList<SpatNodeListener> spatNodeListeners;
	void addLooperListener(SpatNodeListener* newListener) { spatNodeListeners.add(newListener); }
	void removeLooperListener(SpatNodeListener* listener) { spatNodeListeners.remove(listener); }


	ConnectableNodeUI * createUI() override;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpatNode)
};


#endif  // SPATNODE_H_INCLUDED
