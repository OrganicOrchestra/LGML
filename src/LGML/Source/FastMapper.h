/*
  ==============================================================================

    FastMapper.h
    Created: 17 May 2016 6:05:19pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef FASTMAPPER_H_INCLUDED
#define FASTMAPPER_H_INCLUDED

#include "ControllableContainer.h"
#include "FastMap.h"

#include "LGMLDragger.h"
#include "Inspector.h"


class FastMapper;


class FastMapper :
public ControllableContainer,
private LGMLDragger::Listener,
private Inspector::InspectorListener

{
public:
	juce_DeclareSingleton(FastMapper,true)

	FastMapper();
	virtual ~FastMapper();

	OwnedArray<FastMap> maps;
  ParameterProxy * potentialIn;
  ParameterProxy * potentialOut;
  BoolParameter * autoAdd;

	void clear();

  void setPotentialInput(Parameter*);
  void setPotentialOutput(Parameter*);

	FastMap * addFastMap();
	void removeFastmap(FastMap * f);

	virtual var getJSONData() override;
  ControllableContainer *  addContainerFromVar(const String & name,const var & fData) override;

private:

  // LGMLDragger Listener
  void selectionChanged(Controllable *) override;

  // Inspector Component
  void currentComponentChanged(Inspector * ) override;
  ControllableContainer * selectedContainerToListenTo;

  void setContainerToListen(ControllableContainer *);


  // ControllableContainer::Listener
  void controllableFeedbackUpdate(ControllableContainer *,Controllable *) override;
  void createNewFromPotentials();
  bool checkDuplicates(FastMap *f);
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FastMapper)

};


#endif  // FASTMAPPER_H_INCLUDED
