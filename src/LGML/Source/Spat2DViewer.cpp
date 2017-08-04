/*
  ==============================================================================

    Spat2DViewer.cpp
    Created: 29 Sep 2016 7:25:11pm
    Author:  bkupe

  ==============================================================================
*/

#include "Spat2DViewer.h"
#include "Style.h"

Spat2DViewer::Spat2DViewer(Spat2DNode * _node) : node(_node)
{
	updateNumTargets();
	updateNumSources();
	node->addConnectableNodeListener(this);


	if (node->useGlobalTarget->boolValue())
	{
		globalTarget = new Spat2DTarget(-1, Colours::green);
		addAndMakeVisible(globalTarget);
		globalTarget->radius = node->globalTargetRadius->floatValue();
		globalTarget->addHandleListener(this);
		globalTarget->setPosition(node->globalTargetPosition->getPoint());
	}
}

Spat2DViewer::~Spat2DViewer()
{
	node->removeConnectableNodeListener(this);
	sources.clear();
	targets.clear();
}

void Spat2DViewer::updateNumSources()
{
	int numSources = node->numSpatInputs->intValue();
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

	for (int i = 0; i < numSources; i++)
	{
		updateSourcePosition(i);
	}

	resized();
}

void Spat2DViewer::updateNumTargets()
{
	int numTargets = node->numSpatOutputs->intValue();
	for (int i = 0; i < targets.size(); i++)
	{
		targets[i]->removeHandleListener(this);
		removeChildComponent(targets[i]);

	}

	targets.clear();

	for (int i = 0; i < numTargets; i++)
	{
		Spat2DTarget * t = new Spat2DTarget(i);
		t->radius = node->targetRadius->floatValue();
		t->addHandleListener(this);
		addAndMakeVisible(t);
		targets.add(t);
		updateTargetInfluence(i);
	}

	for (int i = 0; i < numTargets; i++)
	{
		updateTargetPosition(i);
	}

	resized();
}

void Spat2DViewer::updateTargetRadius()
{
	for (auto &t : targets)
	{
		t->radius = node->targetRadius->floatValue();
	}

	resized();

}

void Spat2DViewer::updateSourcePosition(int sourceIndex)
{
	if (sourceIndex == -1 || sourceIndex >= sources.size()) return;
	sources[sourceIndex]->setPosition(Point<float>(node->inputDatas[sourceIndex]->elements[0]->value,node->inputDatas[sourceIndex]->elements[1]->value));
}

void Spat2DViewer::updateTargetPosition(int targetIndex)
{
	if (targetIndex == -1 || targetIndex >= targets.size()) return;
	targets[targetIndex]->setPosition(node->targetPositions[targetIndex]->getPoint());
}

void Spat2DViewer::updateTargetInfluence(int targetIndex)
{
	if (targetIndex == -1 || targetIndex >= targets.size()) return;
	targets[targetIndex]->influence = node->outputDatas[targetIndex]->elements[0]->value;
	targets[targetIndex]->repaint();
}

void Spat2DViewer::resized()
{
	Rectangle<int> r = getBounds();
	for (auto & s : sources)
	{
		s->setBounds(s->position.getX()*r.getWidth() - s->size / 2-5, s->position.getY()*r.getHeight() - s->size/2-5, s->size+10, s->size+10);
		s->toFront(false); //keep sources on top
	}


	if (globalTarget != nullptr)
	{
		float gts = globalTarget->radius * 2 * getWidth();
		globalTarget->setBounds(globalTarget->position.getX()*r.getWidth() - gts / 2 - 5, globalTarget->position.getY()*r.getHeight() - gts / 2 - 5, gts + 10, gts + 10);
	}

	for (auto & t : targets)
	{
		float ts = t->radius * 2 * getWidth();
		t->setBounds(t->position.getX()*r.getWidth() - ts/2-5, t->position.getY()*r.getHeight() - ts/2-5, ts+10, ts+10);
	}
}

void Spat2DViewer::paint(Graphics & g)
{
	Component::paint(g);
	g.setColour(BG_COLOR.darker());
	g.fillRect(getLocalBounds());
}

void Spat2DViewer::nodeParameterChanged(ConnectableNode *, Parameter * p)
{
	DBG("node parameter changed " << p->niceName);
	if (p == node->numSpatInputs) updateNumSources();
	else if (p == node->numSpatOutputs) updateNumTargets();
	else if (p == node->targetRadius) updateTargetRadius();
	else if (p->type == Parameter::POINT2D)
	{
		if(p == node->globalTargetPosition)
		{
			if (globalTarget != nullptr)
			{
				globalTarget->setPosition(node->globalTargetPosition->getPoint());
				resized();
			}
		} else
		{
			Point2DParameter<float> * p2d = (Point2DParameter<float> *)p;
			int index = node->targetPositions.indexOf(p2d);
			updateTargetPosition(index);
		}
		
	} else if (p == node->shapeMode)
	{
		bool circleMode = (int)node->shapeMode->getFirstSelectedValue() == Spat2DNode::ShapeMode::CIRCLE;
		for (int i = 0; i < targets.size(); i++) targets[i]->setEnabled(!circleMode);

	} else if (p == node->useGlobalTarget)
	{
		if (node->useGlobalTarget->boolValue())
		{
			globalTarget = new Spat2DTarget(-1, Colours::green);
			addAndMakeVisible(globalTarget);
			globalTarget->radius = node->globalTargetRadius->floatValue();
			globalTarget->setPosition(node->globalTargetPosition->getPoint());
			globalTarget->addHandleListener(this);
		} else
		{
			globalTarget->removeHandleListener(this);
			removeChildComponent(globalTarget);
			globalTarget = nullptr;
		}
		resized();

	}  else if (p == node->globalTargetRadius)
	{
		if (globalTarget != nullptr)
		{
			globalTarget->radius = node->globalTargetRadius->floatValue();
			resized();
		}
	}
}



void Spat2DViewer::nodeInputDataChanged(ConnectableNode *, Data * d)
{
	int index = node->inputDatas.indexOf(d);
	updateSourcePosition(index);
}

void Spat2DViewer::nodeOutputDataUpdated(ConnectableNode *, Data * d)
{
	int index = node->outputDatas.indexOf(d);
	updateTargetInfluence(index);
}

void Spat2DViewer::dataInputAdded(ConnectableNode *, Data *)
{
	updateNumSources();
}

void Spat2DViewer::dataInputRemoved(ConnectableNode *, Data *)
{
	updateNumSources();
}

void Spat2DViewer::controllableAdded(ControllableContainer *,Controllable * c)
{
	if (c->type == Controllable::POINT2D) updateNumTargets();
}

void Spat2DViewer::controllableRemoved(ControllableContainer *,Controllable * c)
{
	if (c->type == Controllable::POINT2D) updateNumTargets();
}

void Spat2DViewer::handleUserMoved(Spat2DHandle * handle, const Point<float> &newPos)
{
	if (handle->type == Spat2DHandle::HandleType::SOURCE)
	{
		node->setSourcePosition(handle->index, newPos);
	} else
	{
		node->setTargetPosition(handle->index, newPos);
	}

}


Spat2DSource::Spat2DSource(int _index): Spat2DHandle(HandleType::SOURCE,_index,25,Colours::grey)
{
	angle = 0;
}

Spat2DTarget::Spat2DTarget(int _index, Colour c) : Spat2DHandle(HandleType::TARGET,_index,20,c)
{
	radius = .25f;
}

void Spat2DTarget::paint(Graphics & g)
{
	Spat2DHandle::paint(g);
	Rectangle<int> r = getLocalBounds();
	Component * parent = getParentComponent();
	float maxRad = parent->getWidth()*radius * 2;

	Colour c = isMouseOver() ? Colours::yellow : color;
	g.setColour(color.darker(.3f).withAlpha(.05f));
	g.fillEllipse(r.withSizeKeepingCentre(maxRad, maxRad).toFloat());
	g.setColour(color.brighter(.3f).withAlpha(.2f));
	g.fillEllipse(r.withSizeKeepingCentre(maxRad*influence, maxRad*influence).toFloat());
	g.setColour(c.withAlpha(.5f));
	g.drawEllipse(r.withSizeKeepingCentre(maxRad, maxRad).toFloat(), 1);
}

Spat2DHandle::Spat2DHandle(HandleType _type, int _index, float _size, Colour _color) : type(_type), index(_index), size(_size), color(_color)
{
	setRepaintsOnMouseActivity(true);
}

Spat2DHandle::~Spat2DHandle()
{
}

void Spat2DHandle::paint(Graphics & g)
{
	Rectangle<int> r = getLocalBounds().withSizeKeepingCentre(size,size).reduced(2);
	Colour c = isMouseOver() ? Colours::yellow : color;
	g.setColour(c.withAlpha(.5f));
	g.fillEllipse(r.toFloat());
	g.setColour(Colours::white);
	g.drawFittedText(String(index), r, Justification::centred, 1);
}

void Spat2DHandle::mouseDown(const MouseEvent & e)
{
	if (e.mods.isRightButtonDown())
	{
		handleListeners.call(&Listener::handleUserMoved, this, Point<float>(.5f,.5f));
	}
	toFront(true);
}

void Spat2DHandle::mouseDrag(const MouseEvent & e)
{
	Component * parent = getParentComponent();
	if (e.mods.isLeftButtonDown())
	{
		Point<float> newPos = Point<float>(jlimit<float>(0, 1, parent->getMouseXYRelative().x*1. / parent->getWidth()), jlimit<float>(0, 1, parent->getMouseXYRelative().y*1. / parent->getHeight()));
		handleListeners.call(&Listener::handleUserMoved, this, newPos);
	}
}

void Spat2DHandle::resized()
{
	repaint();
}

void Spat2DHandle::setPosition(Point<float> newPosition)
{
	Component * parent = getParentComponent();
	position.setXY(newPosition.x,newPosition.y);
	setCentrePosition(position.x*parent->getWidth(), position.y*parent->getHeight());
}

bool Spat2DHandle::hitTest(int x, int y) {
	if (!isEnabled()) return false;

//	Component * parent = getParentComponent();
	Point<float> relPoint(x, y);
	float dist = relPoint.getDistanceFrom(getLocalBounds().getCentre().toFloat());
	return dist < size / 2;
}


