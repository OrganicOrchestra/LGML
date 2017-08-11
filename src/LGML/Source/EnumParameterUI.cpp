/*
 ==============================================================================

 EnumParameterUI.cpp
 Created: 29 Sep 2016 5:35:12pm
 Author:  bkupe

 ==============================================================================
 */

#include "EnumParameterUI.h"



EnumParameterUI::EnumParameterUI(Parameter * parameter) :
ParameterUI(parameter),
ep((EnumParameter *)parameter)
{
  cb.addListener(this);
  cb.setTextWhenNoChoicesAvailable(ep->niceName);
  cb.setTextWhenNothingSelected(ep->niceName);
  cb.setTooltip(ParameterUI::getTooltip());
  cb.setEditableText(ep->userCanEnterText);
  addAndMakeVisible(cb);
  ep->addAsyncEnumParameterListener(this);
  setIsFileBased( false);
  addFunction = nullptr;

  updateComboBox();
}

EnumParameterUI::~EnumParameterUI()
{
  ep->removeAsyncEnumParameterListener(this);
  cb.removeListener(this);
}


void EnumParameterUI::setIsFileBased(bool _isFileBased){
  if(_isFileBased){
    addFunction = [](EnumParameter * ep){
      FileChooser fc("choose file : "+ep->niceName);
      bool res = false;
      Identifier key;
      String value;
      if(fc.browseForFileToOpen()){
        File f( fc.getResult());
        if(f.exists()){
          res = true;
          key = f.getFileNameWithoutExtension();
          value =f.getFullPathName();
        }
      }
      return std::tuple<bool,Identifier,var>(res,key,value);
    };
  }
  else{
    addFunction = nullptr;
  }
}

void EnumParameterUI::updateComboBox()
{
  cb.clear(dontSendNotification);


  idKeyMap.clear();
  if(DynamicObject * dob = ep->getModel()){
    int id = 1;
    cb.addItem("None", NoneId);
    NamedValueSet map = dob->getProperties();
    for(auto & kv:map)
    {
      String key =kv.name.toString();
      String displayed = key;//+" ["+kv.value.toString()+"]";
      cb.addItem(displayed, id);
      idKeyMap.set(id, key);
      keyIdMap.set(key, id);
      id++;
    }
  }
  String sel = ep->getFirstSelectedId().toString();
  selectString(sel);

  if(ep->isEditable){
    cb.addItem("add " + ep->niceName,addElementId);
    cb.addItem("remove " + ep->niceName, removeElementId);
  }
}

String EnumParameterUI::getCBSelectedKey()
{
  if(cb.getSelectedId()<0){
    jassertfalse;
    return String::empty;
  }
  return idKeyMap[cb.getSelectedId()];
}

void EnumParameterUI::resized()
{
  cb.setBounds(getLocalBounds());
}

void EnumParameterUI::enumOptionAdded(EnumParameter *, const Identifier &)
{
  updateComboBox();
}

void EnumParameterUI::enumOptionRemoved(EnumParameter *, const Identifier &)
{
  updateComboBox();
}
void EnumParameterUI::enumOptionSelectionChanged(EnumParameter *,bool isSelected,bool isValid, const Identifier &name){
  if(isValid){
    jassert(keyIdMap.contains(name.toString()));
    cb.setSelectedId(isSelected?keyIdMap[name.toString()]:0,dontSendNotification);
  }
}

void EnumParameterUI::valueChanged(const var & value)
{
  if(value.isString()){
    selectString(value.toString());
  }
  else if (value.isObject()){
    updateComboBox();
  }
}

void EnumParameterUI::comboBoxChanged(ComboBox *)
{
  int id = cb.getSelectedId();
  if (id <=0){
    if(id==0 ){
      String v = cb.getText();
      if(v.isNotEmpty()&& (v != cb.ComboBox::getTextWhenNothingSelected())){
        jassert(ep->userCanEnterText);
        ep->addOption(v,v);
        ep->selectId(v, true,false);
      }
    }
    else if(id==addElementId ){
      if(addFunction!=nullptr){
        auto res = addFunction(ep);
        if(std::get<0>(res)){
          ep->addOption(std::get<1>(res),std::get<2>(res));
          ep->selectId(std::get<1>(res), true,false);
        }
      }
      else if (id==removeElementId){
        AlertWindow nameWindow("which element should be added ?", "type the elementName", AlertWindow::AlertIconType::NoIcon, this);
        nameWindow.addTextEditor("paramToAdd", parameter->stringValue());
        nameWindow.addButton("OK", 1, KeyPress(KeyPress::returnKey));
        nameWindow.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey));

        int result = nameWindow.runModalLoop();

        if (result)
        {
          Identifier elemToAdd = nameWindow.getTextEditorContents("paramToAdd");
          ep->getModel()->addOption(elemToAdd, elemToAdd.toString());
          ep->selectId(elemToAdd, true,false);
        }


      }
    }
    else if(id==removeElementId){

      AlertWindow nameWindow("which element should be removed ?", "type the elementName", AlertWindow::AlertIconType::NoIcon, this);
      nameWindow.addTextEditor("paramToRemove", ep->getFirstSelectedId().toString());
      nameWindow.addButton("OK", 1, KeyPress(KeyPress::returnKey));
      nameWindow.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey));

      int result = nameWindow.runModalLoop();

      if (result)
      {
        String elemToRemove = nameWindow.getTextEditorContents("paramToRemove");
        if(elemToRemove.isNotEmpty()){
          ep->getModel()->removeOption(elemToRemove);
        }
      }
    }
    else if( id==NoneId){
      cb.setSelectedId(0);
    }

  }
  else{

    ep->setValue(getCBSelectedKey());
  }
};

void EnumParameterUI::selectString(const juce::String & s){
  if(keyIdMap.contains(s)){
    cb.setSelectedId(keyIdMap[s], dontSendNotification);
    cb.setTextWhenNothingSelected(ep->niceName);
  }
  else if(s!=""){
    cb.setTextWhenNothingSelected("["+s+"]");
  }
}


