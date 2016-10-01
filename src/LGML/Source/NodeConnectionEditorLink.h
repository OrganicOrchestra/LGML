/*
  ==============================================================================

    NodeConnectionEditorLink.h
    Created: 29 Mar 2016 7:00:07pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef NODECONNECTIONEDITORLINK_H_INCLUDED
#define NODECONNECTIONEDITORLINK_H_INCLUDED


#include "NodeConnectionEditorDataSlot.h"//keep
//==============================================================================
/*
*/
class NodeConnectionEditorLink    : public Component, 
	public SettableTooltipClient
{
public:
    NodeConnectionEditorLink(NodeConnectionEditorDataSlot * outSlot, NodeConnectionEditorDataSlot * inSlot);
    ~NodeConnectionEditorLink();

    NodeConnectionEditorDataSlot * outSlot;
    NodeConnectionEditorDataSlot * inSlot;

	bool isSelected;
	void setSelected(bool value)
	{
		isSelected = value;
		DBG("repaint");
		repaint();
	}

    bool isEditing;
    void setEditing(bool value)
    {
        isEditing = value;
        repaint();
    }

    NodeConnectionEditorDataSlot * getBaseSlot()
    {
        return outSlot == nullptr ? inSlot : outSlot;
    }

    NodeConnectionEditorDataSlot * candidateDropSlot;

    bool finishEditing();
    bool setCandidateDropSlot(NodeConnectionEditorDataSlot * connector);
    void cancelCandidateDropSlot();

    virtual bool hitTest(int x, int y) override { return hitPath.contains((float)x, (float)y); }

    Path hitPath;
    void paint (Graphics&)override;
    void resized()override;

	void mouseEnter(const MouseEvent &) override;
	void mouseExit(const MouseEvent &) override;
	void mouseDown(const MouseEvent &) override;

	void mouseDoubleClick(const MouseEvent &) override;
	bool keyPressed(const KeyPress &key) override;

	void remove();

    //Listener
    class LinkListener
    {
    public:
        /** Destructor. */
        virtual ~LinkListener() {}
        virtual void askForRemoveLink(NodeConnectionEditorLink * target) = 0;
		virtual void selectLink(NodeConnectionEditorLink * link) = 0;
    };

    ListenerList<LinkListener> listeners;
    void addLinkListener(LinkListener* newListener) { listeners.add(newListener); }
    void removeLinkListener(LinkListener* listener) { listeners.remove(listener); }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeConnectionEditorLink)
};


#endif  // NODECONNECTIONEDITORLINK_H_INCLUDED
