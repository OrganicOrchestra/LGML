/*
  ==============================================================================

    RuleManager.h
    Created: 4 May 2016 5:05:33pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef RULEMANAGER_H_INCLUDED
#define RULEMANAGER_H_INCLUDED

#include "JuceHeader.h"
#include "RuleListeners.h"
#include "ControllableContainer.h"

class RuleManager : 
	public RuleListener, 
	public ControllableContainer
{
public:
	juce_DeclareSingleton(RuleManager, true)

	RuleManager();
	virtual ~RuleManager();

	OwnedArray<Rule> rules;
	//Rule * selectedRule;

	Rule * addRule(const String &ruleName);
	void removeRule(Rule * _rule);

    void askForRemoveRule(Rule *r) override;

	virtual void clear();

	virtual var getJSONData() override;
	virtual void loadJSONDataInternal(var data) override;

	class  Listener
	{
	public:
		/** Destructor. */
		virtual ~Listener() {}

		virtual void ruleAdded(Rule *) = 0;
		virtual void ruleRemoved(Rule *) = 0;
	};

	ListenerList<Listener> ruleManagerListeners;
	void addRuleManagerListener(Listener* newListener) { ruleManagerListeners.add(newListener); }
	void removeRuleManagerListener(Listener* listener) { ruleManagerListeners.remove(listener); }

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RuleManager)
};



#endif  // RULEMANAGER_H_INCLUDED
