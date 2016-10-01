/*
  ==============================================================================

    ShapeShifterFactory.cpp
    Created: 18 May 2016 11:45:57am
    Author:  bkupe

  ==============================================================================
*/

#include "ShapeShifterFactory.h"

#include "NodeManagerUI.h"
#include "TimeManagerUI.h"
#include "Inspector.h"
#include "LGMLLoggerUI.h"
#include "ControllerManagerUI.h"
#include "RuleManagerUI.h"
#include "FastMapperUI.h"
#include "MainComponent.h"


ShapeShifterContent * ShapeShifterFactory::createContentForIndex(PanelName pn)
{
	String contentName = globalPanelNames[(int)pn];

	switch (pn)
	{
	case NodeManagerPanel:
		return new NodeManagerUIViewport(contentName, new NodeManagerUI(NodeManager::getInstance()));
		

	case TimeManagerPanel:
		return new TimeManagerUI(contentName, TimeManager::getInstance());


	case InspectorPanel:
		return new InspectorViewport(contentName, Inspector::getInstance());


	case LoggerPanel:
		return new LGMLLoggerUI(contentName, LGMLLogger::getInstance());


	case ControllerPanel:
		return new ControllerManagerUI(contentName, ControllerManager::getInstance());


	case RulesPanel:
		return new RuleManagerUI(contentName, RuleManager::getInstance());


	case FastMapperPanel:
		return new FastMapperViewport(contentName, new FastMapperUI(FastMapper::getInstance()));


	default:
		DBG("Panel not handled : " << contentName << ", index in names = " << globalPanelNames.strings.indexOf(contentName));
	}
	return nullptr;
}

ShapeShifterContent * ShapeShifterFactory::createContentForName(String name)
{
	return createContentForIndex((PanelName)globalPanelNames.strings.indexOf(name));
}
