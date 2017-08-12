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


#ifndef SERIALUIHELPER_H_INCLUDED
#define SERIALUIHELPER_H_INCLUDED


#include "SerialManager.h"

class SerialDeviceChooser : public ComboBox, public SerialManager::SerialManagerListener
{
public:
	SerialDeviceChooser();
	virtual ~SerialDeviceChooser();

	void fillDeviceList();

	// Inherited via SerialManagerListener
	virtual void portAdded(SerialPortInfo * info) override;
	virtual void portRemoved(SerialPortInfo * info) override;
};



#endif  // SERIALUIHELPER_H_INCLUDED
