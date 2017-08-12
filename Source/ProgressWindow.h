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


#ifndef PROGRESSWINDOW_H_INCLUDED
#define PROGRESSWINDOW_H_INCLUDED

#include "JuceHeader.h"
#include "FloatParameter.h"
#include "FloatSliderUI.h"
#include "Engine.h"
class ProgressWindow : public Component,public ProgressNotifier::ProgressListener
{
public:

	ProgressWindow(const String &title,ProgressNotifier * notifier=nullptr);
	~ProgressWindow();
	

	const int windowWidth = 300;
	const int windowHeight = 100;

	Label titleLabel;
	FloatParameter progressParam;
	ScopedPointer<FloatSliderUI> progressUI;
	
	void paint(Graphics & g) override;
	void resized() override;

  void startedProgress(ProgressTask *  task)override;
  void endedProgress(ProgressTask * task) override;
  void newProgress(ProgressTask *  task ,float advance)override;

	void setProgress(float progress);
};



#endif  // PROGRESSWINDOW_H_INCLUDED
