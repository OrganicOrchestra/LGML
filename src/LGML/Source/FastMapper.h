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


class FastMapper;


class FastMapper : public ControllableContainer
{
public:
	juce_DeclareSingleton(FastMapper,true)

	FastMapper();
	virtual ~FastMapper();

	OwnedArray<FastMap> maps;

	void clear();

	FastMap * addFastMap();
	void removeFastmap(FastMap * f);

	virtual var getJSONData() override;
  void addFromVar(var & fData) override;


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FastMapper)

};


#endif  // FASTMAPPER_H_INCLUDED
