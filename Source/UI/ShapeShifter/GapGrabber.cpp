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


#include "GapGrabber.h"
#include "../Style.h"

GapGrabber::GapGrabber (Direction _direction) : direction (_direction)
{
    setRepaintsOnMouseActivity (true);
}

GapGrabber::~GapGrabber()
{
}

void GapGrabber::paint (Graphics& g)
{
    int grabberSize = 100;
    Rectangle<int> r = getLocalBounds().reduced (2);
    Rectangle<int> tr = (direction == Direction::HORIZONTAL) ? r.withHeight (grabberSize) : r.withWidth (grabberSize);
    tr.setCentre (r.getCentre());
    Colour c = findColour (ResizableWindow::backgroundColourId).brighter (.1f);

    if (isMouseOver()) c = findColour (TextButton::buttonOnColourId);

    if (isMouseButtonDown()) c = c.brighter();

    g.setColour (c);
    g.fillRoundedRectangle (tr.toFloat(), 2);
}

void GapGrabber::mouseEnter (const MouseEvent&)
{
    setMouseCursor (direction == HORIZONTAL ? MouseCursor::LeftRightResizeCursor : MouseCursor::UpDownResizeCursor);
}

void GapGrabber::mouseExit (const MouseEvent&)
{
    setMouseCursor (MouseCursor::NormalCursor);
}

void GapGrabber::mouseDrag (const MouseEvent& e)
{
    listeners.call (&Listener::grabberGrabUpdate, this, direction == HORIZONTAL ? e.getPosition().x : e.getPosition().y);
}
