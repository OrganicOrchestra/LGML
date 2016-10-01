/*
  ==============================================================================

    SerialUIHelper.h
    Created: 22 May 2016 6:35:02pm
    Author:  bkupe

  ==============================================================================
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
