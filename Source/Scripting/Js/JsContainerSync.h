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


#ifndef JSCONTAINERSYNC_H_INCLUDED
#define JSCONTAINERSYNC_H_INCLUDED

#include "../../Controllable/ControllableContainer.h"
//allow to keep structural changes from containers with js namespace
class JsContainerSync: private ControllableContainerListener
{

public:

    JsContainerSync(): aggregChanges (this) {};
    virtual ~JsContainerSync();


    // should return current env
    virtual DynamicObject::Ptr getEnv() = 0;

    void    linkToControllableContainer (const String& jsNamespace, ControllableContainer* c);
    bool existInContainerNamespace (const String&);


    bool isDirty();


protected :

    void childStructureChanged (ControllableContainer*, ControllableContainer* )override;
    void childAddressChanged (ControllableContainer* c) override;

    void updateControllableNamespace (ControllableContainer* c);


private:

    void removeAllListeners (ControllableContainer* c);

    class JsContainerNamespace
    {
    public:
        JsContainerNamespace (const String& n, ControllableContainer* c, DynamicObject* o): nsName (n), container (c), jsObject (o) {}
        WeakReference<ControllableContainer> container;
        DynamicObject::Ptr jsObject;
        String nsName;

    };
    JsContainerNamespace* getContainerNamespace (ControllableContainer*);
    JsContainerNamespace* getContainerNamespace (const String& );


    DynamicObject*   createDynamicObjectFromContainer (ControllableContainer* c, DynamicObject* parent);

    class AggregChanges : private Timer
    {
    public:
        AggregChanges (JsContainerSync* _owner): owner (_owner)
        {
            startTimer (1000);
        }

        void timerCallback() override
        {
            if (nsToUpdate.size())
            {
                Array<JsContainerNamespace* > processed;

                for (auto& ns : nsToUpdate)
                {
                    if (!processed.contains (ns))
                    {
                        owner->getEnv()->setProperty (ns->nsName, owner->createDynamicObjectFromContainer (ns->container, nullptr));
                        processed.add (ns);
                    }
                }

                nsToUpdate.clear();
            }
        }
        void addNs (JsContainerNamespace* ns)
        {
            if (ns == nullptr)
            {
                return;
            }

            nsToUpdate.addIfNotAlreadyThere (ns);
        }
        Array<JsContainerNamespace* > nsToUpdate;
        JsContainerSync* owner;
    };

    AggregChanges aggregChanges;

    OwnedArray<JsContainerNamespace>  linkedContainerNamespaces;

};





#endif  // JSCONTAINERSYNC_H_INCLUDED
