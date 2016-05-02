/*
  ==============================================================================

    GapGrabber.h
    Created: 2 May 2016 5:40:46pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef GAPGRABBER_H_INCLUDED
#define GAPGRABBER_H_INCLUDED

#include "JuceHeader.h"

class GapGrabber : public Component
{
public:
	enum Direction { HORIZONTAL, VERTICAL };

	GapGrabber(Direction _direction);
	virtual ~GapGrabber();

	void paint(Graphics & g) override;
	void mouseEnter(const MouseEvent &e) override;
	void mouseExit(const MouseEvent &e) override;
	void mouseDrag(const MouseEvent &e) override;

	Direction direction;

	//Listener
	class Listener
	{
	public:
		virtual ~Listener() {}
		virtual void grabberGrabUpdate(GapGrabber *, float relativeDist) = 0;
	};

	ListenerList<Listener> listeners;
	void addGrabberListener(Listener* newListener) { listeners.add(newListener); }
	void removeGrabberListener(Listener* listener) { listeners.remove(listener); }
};

#endif  // GAPGRABBER_H_INCLUDED
