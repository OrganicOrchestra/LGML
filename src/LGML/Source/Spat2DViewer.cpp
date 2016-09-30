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
	setNumSources(node->numSpatInputs->intValue());
	setNumTargets(node->numSpatOutputs->intValue());
	repaint();
}

Spat2DViewer::~Spat2DViewer()
{
	sources.clear();
	targets.clear();
}

void Spat2DViewer::computeInfluences()
{
	DBG("Compute influences");

	if (sources.size() == 0) return;
	//Only one source for now
	if (node->modeIsBeam())
	{

	} else
	{

		for (auto &t : targets)
		{
			float dist = jlimit<float>(0,t->radius,sources[0]->position.getDistanceFrom(t->position));
			t->influence = 1 - (dist / t->radius);
			t->repaint();
		}
	}
}

void Spat2DViewer::setNumSources(int numSources)
{
	for (int i = 0; i < sources.size(); i++)
	{
		sources[i]->removeHandleListener(this);
		removeChildComponent(sources[i]);
	}

	sources.clear();

	for (int i = 0; i < numSources; i++)
	{
		Spat2DSource * s = new Spat2DSource(i);
		s->addHandleListener(this);
		addAndMakeVisible(s);
		sources.add(s);
	}

	resized();
}

void Spat2DViewer::setNumTargets(int numTargets)
{
	for (int i = 0; i < targets.size(); i++)
	{
		targets[i]->removeHandleListener(this);
		removeChildComponent(targets[i]);

	}

	targets.clear();

	for (int i = 0; i < numTargets; i++)
	{
		Spat2DTarget * s = new Spat2DTarget(i);
		s->addHandleListener(this);
		addAndMakeVisible(s);
		targets.add(s);
	}

	resized();
}

void Spat2DViewer::resized()
{
	Rectangle<int> r = getBounds();
	for (auto & s : sources)
	{
		s->setBounds((s->position.getX()*.5f + .5f)*r.getWidth() - s->size / 2, (s->position.getY()*.5f + .5f)*r.getHeight() - s->size/2, s->size, s->size);
	}

	for (auto & t : targets)
	{
		float ts = t->radius * 2 * getWidth();
		t->setBounds((t->position.getX()*.5f + .5f)*r.getWidth() - ts/2, (t->position.getY()*.5f + .5f)*r.getHeight() - ts/2, ts, ts);
	}
	repaint();
}

void Spat2DViewer::paint(Graphics & g)
{
	Component::paint(g);
	g.setColour(BG_COLOR.darker());
	g.fillRect(getLocalBounds());
}

void Spat2DViewer::positionChanged(Spat2DHandle * handle)
{
	if (handle->type == Spat2DHandle::HandleType::SOURCE)
	{
		node->setSourcePosition(handle->index, handle->position);
	} else
	{
		node->setTargetPosition(handle->index, handle->position);
	}

	computeInfluences();
}


Spat2DSource::Spat2DSource(int _index): Spat2DHandle(HandleType::SOURCE,_index,25,Colours::grey)
{
	angle = 0;
}

Spat2DTarget::Spat2DTarget(int _index) : Spat2DHandle(HandleType::TARGET,_index,20,Colours::orange)
{
	radius = .25f;
}

void Spat2DTarget::paint(Graphics & g)
{
	Spat2DHandle::paint(g);
	Rectangle<int> r = getLocalBounds();
	
	g.setColour(color);
	g.drawEllipse(r.withSizeKeepingCentre(r.getWidth(), r.getHeight()).toFloat(),2);
	g.setColour(color.brighter(.3f).withAlpha(.5f));
	g.fillEllipse(r.withSizeKeepingCentre(r.getWidth()*influence, r.getHeight()*influence).toFloat());
}

Spat2DHandle::Spat2DHandle(HandleType _type, int _index, float _size, Colour _color) : type(_type), index(_index), size(_size), color(_color)
{
	Random rnd;
	position.setXY(rnd.nextFloat(), rnd.nextFloat());
}

Spat2DHandle::~Spat2DHandle()
{
}

void Spat2DHandle::paint(Graphics & g)
{
	Rectangle<int> r = getLocalBounds().withSizeKeepingCentre(size,size).reduced(2);
	g.setColour(color.withAlpha(.5f));
	g.fillEllipse(r.toFloat());
	g.setColour(Colours::white);
	g.drawFittedText(String(index), r, Justification::centred, 1);
}

void Spat2DHandle::mouseDown(const MouseEvent & e)
{
	toFront(true);
}

void Spat2DHandle::mouseDrag(const MouseEvent & e)
{
	Component * parent = getParentComponent();
	if (e.mods.isLeftButtonDown())
	{
		position.setXY(jlimit<float>(0, 1, parent->getMouseXYRelative().x*1. / parent->getWidth()), jlimit<float>(0, 1, parent->getMouseXYRelative().y*1. / parent->getHeight()));
		setCentrePosition(position.x*parent->getWidth(), position.y*parent->getHeight());
		handleListeners.call(&Listener::positionChanged, this);
	}
}

void Spat2DHandle::resized()
{
	repaint();
}


