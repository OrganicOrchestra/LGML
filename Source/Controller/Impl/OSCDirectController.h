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


#ifndef OSCDIRECTCONTROLLER_H_INCLUDED
#define OSCDIRECTCONTROLLER_H_INCLUDED

#include "OSCController.h"


class OSCDirectController : public OSCController
{
public:
//    DECLARE_OBJ_TYPE_DEFAULTNAME (OSCDirectController, "OSC")
    OSCDirectController (StringRef name);
    virtual ~OSCDirectController();

    BoolParameter* sendTimeInfo,*fullSync;


    virtual Result processMessageInternal (const OSCMessage& msg) override;



    virtual void onContainerParameterChanged (Parameter* p) override;

    OwnedFeedbackListener<OSCDirectController> pSync;



private:


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OSCDirectController)


    // Inherited via Listener


};


#endif  // OSCDIRECTCONTROLLER_H_INCLUDED
