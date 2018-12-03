/*
  ==============================================================================

    FileParameterUI.h
    Created: 2 Dec 2018 3:26:27pm
    Author:  Martin Hermant

  ==============================================================================
*/

#pragma once
#include "../FileParameter.h"
#include "ParameterUI.h"

class FileParameterUI : public ParameterUI,public FilenameComponentListener,private Button::Listener,private FileParameter::Listener{
public:
    FileParameterUI (FileParameter* p);
    ~FileParameterUI();
    void valueChanged (const var& v) override;


    void resized()override;

    FileParameter * getFileParameter() const{return dynamic_cast<FileParameter*>(parameter.get());}
    const File  getCurrentFile() const;
    private:


        /** This method is called after the FilenameComponent's file has been changed. */
    void filenameComponentChanged (FilenameComponent* fileComponentThatHasChanged) override;


private:
    void buttonClicked (Button*) override;
    FilenameComponent fnComponent;
    TextButton options;
    DrawablePath  errorLed;

    /// FileParameter Listener
    void watchStateChanged(FileParameter *) override;
    void loadingStarted(FileParameter * )override;
    void loadingEnded(FileParameter * )override;

};
