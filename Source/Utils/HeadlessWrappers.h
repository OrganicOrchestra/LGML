/*
 ==============================================================================

 Copyright © Organic Orchestra, 2017

 This file is part of LGML. LGML is a software to manipulate sound in real-time

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation (version 3 of the License).

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 ==============================================================================
 */

#pragma once

#if !ENGINE_HEADLESS
#error shouldnt be included in non headless builds
#endif


#include "../../JUCE/modules/juce_gui_extra/misc/juce_RecentlyOpenedFilesList.h"

// override FileBasedDoc without UI
class FileBasedDocument: public ChangeBroadcaster{
public:
    FileBasedDocument (const String& fileExtension_,
                       const String& fileWildcard_,
                       const String& openFileDialogTitle_,
                       const String& saveFileDialogTitle_)
    : changedSinceSave (false),
    fileExtension (fileExtension_),
    fileWildcard (fileWildcard_),
    openFileDialogTitle (openFileDialogTitle_),
    saveFileDialogTitle (saveFileDialogTitle_){

    }
    virtual ~FileBasedDocument(){}

    Result loadFrom (const File& newFile,
                     bool showMessageOnFailure){



        const File oldFile (documentFile);
        documentFile = newFile;

        Result result (Result::fail (TRANS("The file doesn't exist")));

        if (newFile.existsAsFile())
        {
            result = loadDocument (newFile);

            if (result.wasOk())
            {
                //setChangedFlag (false);

                setLastDocumentOpened (newFile);
                return result;
            }
        }

        documentFile = oldFile;

        if (showMessageOnFailure)
            LOG(String("There was an error while trying to load the file: FLNM")
                .replace ("FLNM", "\n" + newFile.getFullPathName())
                + "\n\n"
                + result.getErrorMessage());

        return result;
    }
    void setFile (const File& newFile)
    {
        if (documentFile != newFile)
        {
            documentFile = newFile;
            changed();
        }
    }

    //==============================================================================
    void setChangedFlag (const bool hasChanged)
    {
        if (changedSinceSave != hasChanged)
        {
            changedSinceSave = hasChanged;
            sendChangeMessage();
        }
    }

    void changed()
    {
        changedSinceSave = true;
        sendChangeMessage();
    }

    enum SaveResult
    {
        savedOk = 0,            /**< indicates that a file was saved successfully. */
        userCancelledSave,      /**< indicates that the user aborted the save operation. */
        failedToWriteToFile     /**< indicates that it tried to write to a file but this failed. */
    };

    const File& getFile() const                             { return documentFile; }


    SaveResult saveAs (const File& newFile,
                       const bool warnAboutOverwritingExistingFiles,
                       const bool askUserForFileIfNotSpecified,
                       const bool showMessageOnFailure)
    {
        if (newFile == File())
        {
            // can't save to an unspecified file
            jassertfalse;
            return failedToWriteToFile;
        }

        if (warnAboutOverwritingExistingFiles
            && newFile.exists())
            return userCancelledSave;


        const File oldFile (documentFile);
        documentFile = newFile;

        const Result result (saveDocument (newFile));

        if (result.wasOk())
        {
            setChangedFlag (false);


            sendChangeMessage(); // because the filename may have changed
            return savedOk;
        }

        documentFile = oldFile;


        if (showMessageOnFailure)
            LOG(String("An error occurred while trying to save \"DCNM\" to the file: FLNM")
                .replace ("DCNM", getDocumentTitle())
                .replace ("FLNM", "\n" + newFile.getFullPathName())
                + "\n\n"
                + result.getErrorMessage());

        sendChangeMessage(); // because the filename may have changed
        return failedToWriteToFile;
    }

    virtual String getDocumentTitle()=0;

    virtual Result loadDocument (const File& file)=0;
    virtual Result saveDocument (const File& file)=0;
    virtual File getLastDocumentOpened() =0;
    virtual void setLastDocumentOpened (const File& file)=0 ;

    bool changedSinceSave;
    String fileExtension,fileWildcard;
    String openFileDialogTitle,saveFileDialogTitle;

    File documentFile;

};
