#include "NodeManagerUI.h"
#include "ConnectorComponent.h"

ConnectorComponent::ConnectorComponent(ConnectorIOType ioType, NodeConnection::ConnectionType dataType, NodeBase * node) :
	ioType(ioType), dataType(dataType), node(node)
{
	boxColor = dataType == NodeConnection::ConnectionType::AUDIO ? AUDIO_COLOR : DATA_COLOR;
	setSize(10,10);
}

void ConnectorComponent::mouseDown(const MouseEvent & e)
{
	NodeManagerUI * nmui = getNodeManagerUI();

	if (dataType == NodeConnection::ConnectionType::DATA)
	{
		
		
		String dataName = "";
		String elementName = "";

		if(e.mods.isRightButtonDown())
		{
			int numDatas = 0;
			OwnedArray<DataProcessor::Data>* datas;

			if (ioType == ConnectorIOType::INPUT)
			{
				numDatas = node->dataProcessor->getTotalNumInputData();
				datas = &node->dataProcessor->inputDatas;
			}
			else
			{
				numDatas = node->dataProcessor->getTotalNumOutputData();
				datas = &node->dataProcessor->outputDatas;
			}


			ScopedPointer<PopupMenu> menu = new PopupMenu();

			const int maxElementPerData = 10;

			for (int i = 0; i < numDatas; i++)
			{
				ScopedPointer<PopupMenu> dataMenu = new PopupMenu();
				DataProcessor::Data * d = datas->getUnchecked(i);
				int itemID = i *  maxElementPerData + 1; //max 10 element per data anyway, 1 to not start from 0
			
			
				for (int j = 0; j < d->elements.size(); j++)
				{
					dataMenu->addItem(itemID + (j + 1), d->elements[j]->name);
				}
				menu->addSubMenu(d->name, *dataMenu, true, nullptr, false, itemID);
			}



			DBG("here");
			int resultID = menu->show();

			int offsetID = resultID - 1;

			int dataID = (int)floor(offsetID / 10);
			int elementID = offsetID % maxElementPerData;

			dataName = datas->getUnchecked(dataID)->name;
			if (elementID > 0) elementName = datas->getUnchecked(dataID)->elements[elementID-1]->name;

		}

		nmui->createDataConnectionFromConnector(this,dataName,elementName);
	}
	else
	{
		nmui->createAudioConnectionFromConnector(this, 0);
	}
}

void ConnectorComponent::mouseDrag(const MouseEvent & e)
{
	getNodeManagerUI()->updateEditingConnection();
}

void ConnectorComponent::mouseUp(const MouseEvent & e)
{
	getNodeManagerUI()->finishEditingConnection(this);
}


NodeManagerUI * ConnectorComponent::getNodeManagerUI() const noexcept
{
	return findParentComponentOfClass<NodeManagerUI>();
}

NodeBaseUI * ConnectorComponent::getNodeUI() const noexcept
{
	return findParentComponentOfClass<NodeBaseUI>();
}
