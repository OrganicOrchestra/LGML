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


#include "EnumParameterUI.h"

#include "../UndoableHelper.h"

EnumParameterUI::EnumParameterUI (Parameter* parameter) :
    ParameterUI (parameter),
    ep ((EnumParameter*)parameter),
    lastId (NoneId),
    isSorted(true)
{
    addMouseListener(this, true);
    cb.addListener (this);
    cb.setTextWhenNoChoicesAvailable ("No choices for" + ep->niceName);
    cb.setTextWhenNothingSelected (ep->niceName);
    cb.setTooltip (ParameterUI::getTooltip());
    cb.setEditableText (ep->userCanEnterText);
    addAndMakeVisible (cb);
    ep->addAsyncEnumParameterListener (this);


    updateComboBox();
}

EnumParameterUI::~EnumParameterUI()
{
    if(parameter.get()){
        ep->removeAsyncEnumParameterListener (this);
    }
    cb.removeListener (this);
}

void EnumParameterUI::mouseDown(const MouseEvent & e){
    if(e.mods.isLeftButtonDown())
        ep->getModel()->refresh();
}

void EnumParameterUI::updateComboBox()
{
    cb.clear (dontSendNotification);


    keyIdMap.clear();
    idKeyMap.clear();

    if (EnumParameterModel* mod = ep->getModel())
    {
        int id = 1;
        cb.addItem ("None", NoneId);
        NamedValueSet map = mod->getProperties();
        StringArray keys ;
        for(auto & k:map){
            keys.add(k.name.toString());
        }
        if(isSorted){keys.sort(true);}


        for (auto& key : keys)
        {
            
//            String key = kv.name.toString();
            String displayed = key;//+" ["+kv.value.toString()+"]";
            cb.addItem (displayed, id);
            idKeyMap.set (id, key);
            keyIdMap.set (key, id);
            id++;
        }
        
    }

    
    String sel = ep->getFirstSelectedId().toString();
//    DBG("enum CB Select " << sel << " ; " << ep->shortName);
    selectString (sel);

    if (ep->isEditable)
    {
        cb.addItem ("add " + ep->niceName, addElementId);
        cb.addItem ("remove " + ep->niceName, removeElementId);
    }
}

String EnumParameterUI::getCBSelectedKey()
{
    if (cb.getSelectedId() < 0)
    {
        jassertfalse;
        return String::empty;
    }

    return idKeyMap[cb.getSelectedId()];
}

void EnumParameterUI::resized()
{
    cb.setBounds (getLocalBounds());
}

void EnumParameterUI::enumOptionAdded (EnumParameter*, const Identifier&)
{
    updateComboBox();
}

void EnumParameterUI::enumOptionRemoved (EnumParameter*, const Identifier&)
{
    updateComboBox();
}
void EnumParameterUI::enumOptionSelectionChanged (EnumParameter*, bool _isSelected, bool isValid, const Identifier& name)
{
    DBG ("enum change : " << name.toString() << (!_isSelected ? " not" : "") << " selected " << (!isValid ? "in-" : "") << "valid");

    if (isValid)
    {
        jassert (keyIdMap.contains (name.toString()));
        cb.setSelectedId (_isSelected ? keyIdMap[name.toString()] : 0, dontSendNotification);
    }
}

void EnumParameterUI::valueChanged (const var& value)
{
    if (value.isString())
    {
        selectString (value.toString());
    }
    else if (value.isObject())
    {
        updateComboBox();
    }
}

void EnumParameterUI::comboBoxChanged (ComboBox* c)
{
    jassert (c == &cb);
    int id = cb.getSelectedId();

    if (id <= 0)
    {
        if (id == 0 )
        {
            String v = cb.getText();

            if (v.isNotEmpty() && (v != cb.ComboBox::getTextWhenNothingSelected()))
            {
                jassert (ep->userCanEnterText);
                ep->addOrSetOption (v, v, true);
                UndoableHelpers::setValueUndoable(ep, v);
            }
            else
            {
                UndoableHelpers::setValueUndoable(ep, var::undefined());

            }
        }
        else if (id == addElementId )
        {
            if (auto addFunction = ep->getModel()->addFunction)
            {

                auto res = addFunction (ep);

                if (std::get<0> (res))
                {
                    ep->addOption (std::get<1> (res), std::get<2> (res));
                    ep->selectId (std::get<1> (res), true, false);
                }
                else
                {
                    cb.setSelectedId (lastId, dontSendNotification);
                }
            }
            else
            {
                AlertWindow nameWindow ("which element should be added ?", "type the elementName", AlertWindow::AlertIconType::NoIcon, this);
                nameWindow.addTextEditor ("paramToAdd", parameter->stringValue());
                nameWindow.addButton ("OK", 1, KeyPress (KeyPress::returnKey));
                nameWindow.addButton ("Cancel", 0, KeyPress (KeyPress::escapeKey));

                int result = nameWindow.runModalLoop();
                bool changedFromUI = false;

                if (result)
                {
                    String res = nameWindow.getTextEditorContents ("paramToAdd");

                    if (res.isNotEmpty())
                    {
                        Identifier elemToAdd = res;
                        ep->getModel()->addOption (elemToAdd, elemToAdd.toString(), true);
                        ep->selectId (elemToAdd, true, false);
                        changedFromUI = true;
                    }
                }

                if (!changedFromUI)
                {
                    cb.setSelectedId (lastId, dontSendNotification);
                }


            }
        }
        else if (id == removeElementId)
        {

            AlertWindow nameWindow ("which element should be removed ?", "type the elementName", AlertWindow::AlertIconType::NoIcon, this);
            nameWindow.addTextEditor ("paramToRemove", ep->getFirstSelectedId().toString());
            nameWindow.addButton ("OK", 1, KeyPress (KeyPress::returnKey));
            nameWindow.addButton ("Cancel", 0, KeyPress (KeyPress::escapeKey));

            int result = nameWindow.runModalLoop();
            bool changedFromUI = false;

            if (result)
            {
                String elemToRemove = nameWindow.getTextEditorContents ("paramToRemove");

                if (elemToRemove.isNotEmpty())
                {
                    ep->getModel()->removeOption (elemToRemove, false);
                    changedFromUI = true;
                }
            }

            if (!changedFromUI)
            {
                cb.setSelectedId (lastId, dontSendNotification);
            }
            else{
                
                cb.setTextWhenNothingSelected (ep->niceName);
                cb.setSelectedId(NoneId);
                
            }
        }
        else if ( id == NoneId)
        {
            cb.setSelectedId (0);
        }

    }
    else
    {
        UndoableHelpers::startNewTransaction(ep,true);
        UndoableHelpers::setValueUndoable(ep,getCBSelectedKey());
    }
};

void EnumParameterUI::selectString (const juce::String& s)
{
    if (keyIdMap.contains (s))
    {
        lastId = keyIdMap[s];
        cb.setSelectedId (lastId, dontSendNotification);
        cb.setTextWhenNothingSelected (ep->niceName);
    }
    else if (s != "")
    {
        // not valid
        cb.setTextWhenNothingSelected ("[" + s + "]");
    }
    else
    {
        lastId = NoneId;
    }
}


