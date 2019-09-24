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

#pragma once

#include "../../JuceHeaderUI.h"//keep

class ShapeShifterContent;

const static StringArray globalPanelNames = { "Node Manager", "Time Manager", "Inspector", "Logger", "Controllers", "Fast Mapper", "Outliner","Help" };
enum PanelName {NodeManagerPanel, TimeManagerPanel, InspectorPanel, LoggerPanel, ControllerPanel, FastMapperPanel, OutlinerPanel,ToolTipPanel};

class ShapeShifterFactory
{
public:
    static ShapeShifterContent* createContentForIndex (PanelName panelName);
    static ShapeShifterContent* createContentForName (String name);
};



