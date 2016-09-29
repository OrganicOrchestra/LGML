/*
  ==============================================================================

    Spat3DViewer.h
    Created: 29 Sep 2016 7:26:40pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef SPAT3DVIEWER_H_INCLUDED
#define SPAT3DVIEWER_H_INCLUDED


#include "JuceHeader.h"

class Spat3DViewer : public Component
{
public :
	Spat3DViewer();
	~Spat3DViewer();

	void setNumSources(int numSources);
	void setNumTargets(int numTargets);


	void resized() override;
	void paint(Graphics &g) override;
};


#endif  // SPAT3DVIEWER_H_INCLUDED
