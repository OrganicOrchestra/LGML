/*
  ==============================================================================

    ShapeShifterManager.cpp
    Created: 2 May 2016 3:11:35pm
    Author:  bkupe

  ==============================================================================
*/

#include "ShapeShifterManager.h"

juce_ImplementSingleton(ShapeShifterManager);

ShapeShifterManager::ShapeShifterManager() : 
	mainContainer(ShapeShifterContainer::ContentType::CONTAINERS,ShapeShifterContainer::Direction::VERTICAL)
{

}

ShapeShifterManager::~ShapeShifterManager()
{
}
