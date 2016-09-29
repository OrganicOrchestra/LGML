/*
  ==============================================================================

    Spat2DViewer.h
    Created: 29 Sep 2016 7:25:11pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef SPAT2DVIEWER_H_INCLUDED
#define SPAT2DVIEWER_H_INCLUDED

#include "JuceHeader.h"
#include "SpatNode.h"

class Spat2DSource : public Component
{
public:
	Spat2DSource();
	~Spat2DSource();

	void paint(Graphics &g) override;
	void mouseDrag(const MouseEvent &e) override;

	Point<float> position;
	float angle;
};

class Spat2DTarget : public Component
{
public:
	Spat2DTarget();
	~Spat2DTarget();

	void paint(Graphics &g) override;
	void mouseDrag(const MouseEvent &e) override;

	Point<float> position;
	float radius;
};

class Spat2DViewer : public Component
{
public:
	Spat2DViewer(SpatNode * node);
	~Spat2DViewer();

	OwnedArray<Spat2DSource> sources;
	OwnedArray<Spat2DTarget> targets;

	SpatNode * node;
	
	void setNumSources(int numSources);
	void setNumTargets(int numTargets);
	
	void resized() override;
	void paint(Graphics &g) override;
};


#endif  // SPAT2DVIEWER_H_INCLUDED
