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


#ifndef PRESETCHOOSER_H_INCLUDED
#define PRESETCHOOSER_H_INCLUDED

#include "ControllableContainer.h"

class PresetChooser : public ComboBox,
					  public ComboBox::Listener,
				      private ControllableContainerListener
{
public:
	PresetChooser(ControllableContainer * _container);
	~PresetChooser();

	WeakReference<ControllableContainer> container;

	virtual void updatePresetComboBox(bool forceUpdate = false);
	virtual void comboBoxChanged(ComboBox * comboBoxThatHasChanged) override;

	virtual void controllableContainerPresetLoaded(ControllableContainer *) override;

};



#endif  // PRESETCHOOSER_H_INCLUDED
