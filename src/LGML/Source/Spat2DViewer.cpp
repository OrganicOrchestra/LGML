/*
  ==============================================================================

    Spat2DViewer.cpp
    Created: 29 Sep 2016 7:25:11pm
    Author:  bkupe

  ==============================================================================
*/

#include "Spat2DViewer.h"
#include "Style.h"
#include "SpatNode.h"

Spat2DViewer::Spat2DViewer(SpatNode * _node) : node(_node)
{
	repaint();
}

Spat2DViewer::~Spat2DViewer()
{
	sources.clear();
	targets.clear();
}

void Spat2DViewer::setNumSources(int numSources)
{
	sources.clear();
	for (int i = 0; i < numSources; i++) sources.add(new Spat2DSource());
}

void Spat2DViewer::setNumTargets(int numTargets)
{
	targets.clear();
	for (int i = 0; i < numTargets; i++) targets.add(new Spat2DTarget());
}

void Spat2DViewer::resized()
{
	repaint();
}

void Spat2DViewer::paint(Graphics & g)
{
	Component::paint(g);
	g.setColour(BG_COLOR.darker());
	g.fillRect(getLocalBounds());
}


Spat2DSource::Spat2DSource()
{
	position.setXY(0, 0);
}

Spat2DSource::~Spat2DSource()
{
}

void Spat2DSource::paint(Graphics & g)
{
}

void Spat2DSource::mouseDrag(const MouseEvent & e)
{
}

Spat2DTarget::Spat2DTarget()
{
	radius = .1f;
	Random rnd;
	position.setXY(rnd.nextFloat() * 2 - 1, rnd.nextFloat() * 2 - 1);
}

Spat2DTarget::~Spat2DTarget()
{
}

void Spat2DTarget::paint(Graphics & g)
{
}

void Spat2DTarget::mouseDrag(const MouseEvent & e)
{
}


