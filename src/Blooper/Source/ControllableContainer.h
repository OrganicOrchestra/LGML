/*
  ==============================================================================

	ControllableContainer.h
	Created: 8 Mar 2016 1:15:36pm
	Author:  bkupe

  ==============================================================================
*/

#ifndef CONTROLLABLECONTAINER_H_INCLUDED
#define CONTROLLABLECONTAINER_H_INCLUDED

#include "Controllable.h"
#include "FloatParameter.h"
#include "IntParameter.h"
#include "FloatRangeParameter.h"
#include "BoolParameter.h"
#include "StringParameter.h"
#include "Trigger.h"

class ControllableContainer : public Parameter::Listener, public Trigger::Listener
{
public:
	ControllableContainer(const String &niceName);
	~ControllableContainer();

	String niceName;
	String shortName;
	bool hasCustomShortName;

	bool skipControllableNameInAddress;

	void setNiceName(const String &niceName) {
		this->niceName = niceName;
		if (!hasCustomShortName) setAutoShortName();
	}

	void setCustomShortName(const String &shortName)
	{
		this->shortName = shortName;
		hasCustomShortName = true;
	}

	void setAutoShortName() {
		hasCustomShortName = false;
		shortName = StringUtil::toShortName(niceName);
		updateChildrenControlAddress();
	}


	OwnedArray<Controllable> controllables;
	Array<ControllableContainer * > controllableContainers;
	ControllableContainer * parentContainer;

	FloatParameter * addFloatParameter(const String &niceName, const String &description, const float &initialValue, const float &minValue = 0, const float &maxValue = 1, const bool &enabled = true);
	IntParameter * addIntParameter(const String &niceName, const String &description, const int &initialValue, const int &minValue, const int &maxValue, const bool &enabled = true);
	BoolParameter * addBoolParameter(const String &niceName, const String &description, const bool &value, const bool &enabled = true);
	StringParameter * addStringParameter(const String &niceName, const String &description, const String &value, const bool &enabled = true);
	Trigger * addTrigger(const String &niceName, const String &description, const bool &enabled = true);

	void removeControllable(Controllable * c);
	Controllable * getControllableByName(const String &name);

	void addChildControllableContainer(ControllableContainer * container);
	void removeChildControllableContainer(ControllableContainer *container);

	void setParentContainer(ControllableContainer * container);
	void updateChildrenControlAddress();
	Array<Controllable *> getAllControllables(bool recursive = false);

	Controllable * getControllableForAddress(Array<String> addressSplit, bool recursive = true, bool getNotExposed = false);
	

	void dispatchFeedback(Controllable * c);

	// Inherited via Parameter::Listener
	virtual void parameterValueChanged(Parameter * p) override;
	// Inherited via Trigger::Listener
	virtual void triggerTriggered(Trigger * p) override;

private:
	void addParameterInternal(Parameter * p);

public:
	//Listener
	class  Listener
	{
	public:
		/** Destructor. */
		virtual ~Listener() {}
		virtual void controllableAdded(Controllable * c) = 0;
		virtual void controllableRemoved(Controllable * c) = 0;
		virtual void controllableFeedbackUpdate(Controllable *c) = 0;
	};

	ListenerList<Listener> listeners;
	void addControllableContainerListener(Listener* newListener) { listeners.add(newListener); }
	void removeControllableContainerListener(Listener* listener) { listeners.remove(listener); }

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ControllableContainer)

		
};


#endif  // CONTROLLABLECONTAINER_H_INCLUDED
