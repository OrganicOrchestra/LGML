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

#pragma once


class CommandLineElement
{
public:
    CommandLineElement (const String& name = String::empty): command (name) {}
    String command;
    StringArray args;

    bool isEmpty() {return (command == "") && (args.size() == 0);}
    CommandLineElement& operator= (const CommandLineElement& rhs)
    {
        if (this == &rhs)
            return *this;

        command = rhs.command;
        args = rhs.args;
        return *this;
    }
    operator bool()
    {
        return !isEmpty();
    }

};

//typedef Array<CommandLineElement> CommandLineElements;
class CommandLineElements: public Array<CommandLineElement>
{
public:

    CommandLineElement  getCommandLineElement (const String command)
    {
        for (auto& e : *this )
        {
            if (e.command == command)
            {
                return e;
            }
        }

        return CommandLineElement();
    }

    CommandLineElement  getCommandLineElement (const String command, const String command2 )
    {
        for (auto& e : *this )
        {
            if ((e.command == command) || (e.command == command2))
            {
                return e;
            }
        }

        return CommandLineElement();
    }

    bool containsCommand (const String& c)
    {
        for (auto& a : *this)
        {
            if (a.command == c)return true;
        }

        return false;
    }

    static CommandLineElements parseCommandLine (const String& commandLine)
    {
        CommandLineElements res;
        StringArray args;
        args.addTokens (commandLine, true);
        args.trim();

        int parsingIdx = 0;

        while (parsingIdx < args.size())
        {
            String command = "";
            bool isParameter = args[parsingIdx].startsWith ("-");

            if (isParameter)
            {
                command = args[parsingIdx].substring (1, args[parsingIdx].length());
                parsingIdx++;
                res.add (CommandLineElement (command));

                // handles command only args
                if (parsingIdx >= args.size()) {break;}
            }

            String argument = args[parsingIdx].removeCharacters (juce::StringRef ("\""));

            // handles no command args at the begining
            if (res.size() == 0) {res.add (CommandLineElement());}

            (res.end() - 1)->args.add (argument);
            //DBG("parsing commandline, command : " << command << ", argument :" << argument << " / parsingIdx : " << parsingIdx);

            parsingIdx++;
        }

        return res;

    };

};

