/* Copyright © Organic Orchestra, 2017
*
* This file is part of LGML.  LGML is a software to manipulate sound in realtime
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation (version 3 of the License).
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
*/


#include "ShapeShifterFactory.h"

#include "../../Node/Manager/UI/NodeManagerUI.h"
#include "../../Time/TimeManagerUI.h"
#include "../Inspector/Inspector.h"
#include "../../Logger/LGMLLoggerUI.h"
#include "../../Controller/UI/ControllerManagerUI.h"
#include "../../FastMapper/FastMapperUI.h"
#include "../Outliner.h"

#include "../MainComponent.h"
#include "TooltipPanel.h"


ShapeShifterContent* ShapeShifterFactory::createContentForIndex (PanelName pn)
{
    String contentName = globalPanelNames[ (int)pn];

    switch (pn)
    {
        case NodeManagerPanel:
            return new NodeManagerUIViewport (contentName, NodeManagerUI::getInstance ());


        case TimeManagerPanel:
            return new TimeManagerUI (contentName, TimeManager::getInstance());


        case InspectorPanel:
            return new InspectorViewport (contentName, Inspector::getInstance());


        case LoggerPanel:
            return new LGMLLoggerUI (contentName, LGMLLogger::getInstance());


        case ControllerPanel:
            return new ControllerManagerUIViewport (contentName, new ControllerManagerUI ( ControllerManager::getInstance()));



        case FastMapperPanel:
            return new FastMapperViewport (contentName, new FastMapperUI (FastMapper::getInstance()));

        case OutlinerPanel:
            return new Outliner (contentName);

        case ToolTipPanel:
            return new TooltipPanel(contentName);


        default:
            DBG ("Panel not handled : " << contentName << ", index in names = " << globalPanelNames.strings.indexOf (contentName));
    }

    return nullptr;
}

ShapeShifterContent* ShapeShifterFactory::createContentForName (String name)
{
    return createContentForIndex ((PanelName)globalPanelNames.strings.indexOf (name));
}
