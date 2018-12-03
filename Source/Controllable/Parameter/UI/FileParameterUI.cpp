/*
  ==============================================================================

    FileParameterUI.cpp
    Created: 2 Dec 2018 3:26:27pm
    Author:  Martin Hermant

  ==============================================================================
*/

#include "FileParameterUI.h"
#include "../../../Engine.h"

void createFile(FileParameter* fp);
void openFile(FileParameter* fp);

FileParameterUI::FileParameterUI (FileParameter* p): ParameterUI (p), fnComponent (p->niceName,//const String& name,
                                                                  File(),//const File& currentFile,
                                                                  p->isEditable,//canEditFilename),
                                                                  false,//bool isDirectory,
                                                                  true,//,bool isForSaving,
                                                                  "",//const String& fileBrowserWildcard,
                                                                  "",//const String& enforcedSuffix,
                                                                  p->description)//const String& textWhenNothingSelected);
,options()
{
    options.addListener(this);
    addAndMakeVisible(options);
    valueChanged(p->value);
    fnComponent.addListener (this);
    addAndMakeVisible (fnComponent);
    p->addFileListener(this);
    addAndMakeVisible(errorLed);
    loadingEnded(p);


}

FileParameterUI::~FileParameterUI(){
    if(auto * p = getFileParameter())
        p->removeFileListener(this);
}


void reLayout(FileParameterUI *  ui ){

//    reloadT =  addNewParameter<Trigger> ("reloadFile", "reload current file");
//    reloadT->isControllableExposed = false;
//    showT =  addNewParameter<Trigger> ("showFile", "open file in text editor");
//    showT->isControllableExposed = false;
//    autoWatch = addNewParameter<BoolParameter> ("autoWatch", "auto reload if file has been changed", false);
//    autoWatch->isSavable = false;

//    else if (p == loadT)
//    {
//        File  startFolder (jsEnv->getCurrentFile());
//
//        if (startFolder.exists()) {startFolder = startFolder.getParentDirectory();}
//        else {startFolder = getEngine()->getCurrentProjectFolder();}
//
//        FileChooser myChooser ("Please select the script you want to load...",
//                               startFolder,
//                               "*.js");
//
//        if (myChooser.browseForFileToOpen())
//        {
//            File script (myChooser.getResult());
//            jsEnv->loadFile (script);
//        }
//    }
    
}

void FileParameterUI::valueChanged (const var& v)
{
    fnComponent.setCurrentFile(getFileParameter()->getFile(), true);

};

void FileParameterUI::filenameComponentChanged (FilenameComponent* fileComponentThatHasChanged) {
    if(fileComponentThatHasChanged == &fnComponent){
        getFileParameter()->setValueFrom(this,getEngine()->getNormalizedFilePath(fnComponent.getCurrentFile()));
    }
};



void FileParameterUI::resized()
{
    auto r =getLocalBounds();
    options.setBounds(r.removeFromLeft(10));
    if(auto fp = getFileParameter()){
        if(!fp->hasValidPath(true) || fp->getLoadingState()==LOADED_WITH_ERROR){
            errorLed.setBounds(r.removeFromLeft(10));
        }

    }
    fnComponent.setBounds (r);

}

const File  FileParameterUI::getCurrentFile() const{
    if(auto * p = getFileParameter()){
        return p->getFile();
    }
    return File();

}
void FileParameterUI::buttonClicked (Button*) {
    PopupMenu menu;
    auto * fp = getFileParameter();
    if(fp->hasValidPath(false)){
        menu.addItem(1, "reload");
        menu.addItem(2, "show");
        menu.addItem(3, "watch",true,getFileParameter()->isWatching);
    }
    else{
        menu.addItem(4,"create");
        menu.addItem(5,"open");
    }
    auto res = menu.show();
    switch(res){
        case 1:
            if(auto * fp =getFileParameter())
                fp->triggerFileChange();
            break;
        case 2:
            if (getCurrentFile().existsAsFile())
                getCurrentFile().startAsProcess();
        case 3:
            if(auto * fp =getFileParameter())
                fp->setIsWatching(!getFileParameter()->isWatching);
            break;
        case 4:
            createFile(getFileParameter());
            break;
        case 5:
            openFile(getFileParameter());
            break;
    }

}



void FileParameterUI::watchStateChanged(FileParameter * fp) {
    options.setToggleState(fp->isWatching, dontSendNotification);
};


void FileParameterUI::loadingStarted(FileParameter * ){

};
void FileParameterUI::loadingEnded(FileParameter * fp){

    int size = 10;
    Path circle;
    circle.addEllipse (Rectangle<float> (0, 0, (float)size, (float)size));
    errorLed.setPath (circle);


    if(fp->getLoadingState()==LOADED  || fp->stringValue().isEmpty()){
        errorLed.setVisible(false);
    }
    else{
        errorLed.setVisible(true);

    }
    errorLed.setFill (FillType (!fp->hasValidPath(true)? Colours::red:
                                  (fp->getLoadingState()==LOADED_WITH_ERROR ? Colours::orange :
                                   Colours::green)));

    resized();
//    validLed.setFill (FillType ((env->hasValidJsFile() && env->isInSyncWithLGML()) ? Colours::green :
//                                  (env->hasValidJsFile() ? Colours::orange :
//                                   Colours::red)));
};

// TODO add auto show

void show(FileParameter * fp){
    fp->getFile().startAsProcess();
}

ScopedPointer<FileChooser> getFcForFp(FileParameter * fp,bool save){
    return new FileChooser (juce::translate(String(save?"save":"load")+" file"),//    const String& chooserBoxTitle,
                   getEngine()->getCurrentProjectFolder(),//    const File& currentFileOrDirectory,
                   fp->getAllowedExtensionsFilter(true),//    const String& fileFilters,
                   true,//    const bool useNativeBox,
                   false,//    const bool treatFilePackagesAsDirectories,
                nullptr);//    Component* parentComponentToUse
    

}

void openFile(FileParameter *fp){
    auto fc = getFcForFp(fp,false);
    if(fc->browseForFileToOpen()){
        File file = fc->getResult();
        if(file.exists()){
            fp->setFile(file);
        }
    }
}

void createFile(FileParameter *fp){
    if(!getEngine()->getCurrentProjectFolder().exists()){
        auto fc = getFcForFp(fp,true);
        auto res = fc->browseForFileToSave(true);
        if(res){
            File file = fc->getResult();
            Result r = file.create();
            if(!r){LOGE(r.getErrorMessage());}
            else{
                fp->setFile( file);
                show(fp);
            }
        }
        else{
            return;
        }
    }
    else{
        AlertWindow nameWindow (juce::translate("Create File"), juce::translate("name your file"), AlertWindow::AlertIconType::NoIcon);
        nameWindow.addTextEditor ("Name", "MyScript");

        nameWindow.addButton ("OK", 1, KeyPress (KeyPress::returnKey));
        nameWindow.addButton ("Cancel", 0, KeyPress (KeyPress::escapeKey));

        int result = nameWindow.runModalLoop();

        if (result)
        {
            String sN = nameWindow.getTextEditor("Name")->getText();
            if(sN.isNotEmpty()){
                if( sN.endsWith(".js")){
                    sN= sN.substring(0, -2);
                }
                File scriptFile = getEngine()->getCurrentProjectFolder().getChildFile("Scripts/");

                if(! scriptFile.exists()){
                    scriptFile.createDirectory();
                }
                scriptFile = scriptFile.getNonexistentChildFile(sN,".js",true);
                if(! scriptFile.exists()){
                    auto r = scriptFile.create();
                    if(!r){
                        LOGE(juce::translate("Can't create script : ") <<scriptFile.getFullPathName() << "("<< r.getErrorMessage()<<")");
                    }
                }
                else{
                    jassertfalse;
                }

                if(scriptFile.exists()){
                    fp->setFile(scriptFile);
                    show(fp);
                }
            }
            
        }
    }
}

