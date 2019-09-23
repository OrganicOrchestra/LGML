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


#ifndef CONNECTABLENODEAUDIOCTLUI_H_INCLUDED
#define CONNECTABLENODEAUDIOCTLUI_H_INCLUDED
#pragma once



//class ConnectableNode;
//class ConnectableNodeUI;
//
//#include "../../JuceHeaderUI.h"//keep
//#include "../../Controllable/Parameter/UI/SliderUI.h"





class ConnectableNodeAudioCtlUI: public juce::Component
{

public:
    ConnectableNodeAudioCtlUI():
    outputVolume (nullptr)
    //,bypassUI(nullptr)
    {


    }
    

    void setNodeAndNodeUI (ConnectableNode* _node, ConnectableNodeUI* _nodeUI){
            jassert (_node->hasAudioOutputs());
        outputVolume = std::make_unique< FloatSliderUI> (_node->outputVolume);
            outputVolume->orientation = FloatSliderUI::Direction::VERTICAL;
            addAndMakeVisible (outputVolume.get());
            //bypassUI = node->bypass->createToggle();
            //addAndMakeVisible(bypassUI);



    }
    

    void resized() override{
        Rectangle<int > outCtl = getLocalBounds();

        //if(bypassUI)bypassUI->setBounds(outCtl.removeFromTop(outCtl.getWidth()));
        if (outputVolume)outputVolume->setBounds (outCtl);
    }

    std::unique_ptr<SliderUI<floatParamType> >  outputVolume;
    //std::unique_ptr<BoolToggleUI>  bypassUI;



};


#endif  // CONNECTABLENODEAUDIOCTLUI_H_INCLUDED
