#include "NodeManagerUI.h"
#include "ConnectorComponent.h"

ConnectorComponent::ConnectorComponent(ConnectorIOType ioType, NodeConnection::ConnectionType dataType, NodeBase * node) :
	ioType(ioType), dataType(dataType), node(node)
{
	boxColor = dataType == NodeConnection::ConnectionType::AUDIO ? AUDIO_COLOR : DATA_COLOR;
	setSize(10,10);
}

void ConnectorComponent::paint(Graphics & g)
{
	g.setGradientFill(ColourGradient(boxColor, getLocalBounds().getCentreY(),getLocalBounds().getCentreY(), boxColor.darker(), 0,0, true));
	//g.setFillType(FillType::gradient);
	g.fillRoundedRectangle(getLocalBounds().toFloat(), 2);
}

void ConnectorComponent::mouseDown(const MouseEvent & e)
{
	NodeManagerUI * nmui = getNodeManagerUI();

	if (dataType == NodeConnection::ConnectionType::DATA)
	{
		
		
		String dataName = "";
		String elementName = "";
		DataProcessor::DataType dType = DataProcessor::DataType::Unknown;

		if (e.mods.isRightButtonDown())
		{
			selectDataAndElementPopup(dataName, elementName, dType);
			DBG("Select data and element popup : " + dataName + ", " + elementName+", "+String(dType));
		}

		nmui->createDataConnectionFromConnector(this,dataName,elementName, dType);
	}
	else
	{
		nmui->createAudioConnectionFromConnector(this, 0);
	}
}


//void ConnectorComponent::mouseDrag(const MouseEvent & e)
//{
//	getNodeManagerUI()->updateEditingConnection();
//}
//
//void ConnectorComponent::mouseUp(const MouseEvent & e)
//{
//	getNodeManagerUI()->finishEditingConnection(this);
//}


void ConnectorComponent::selectDataAndElementPopup(String & selectedDataName, String & selectedElementName,
	DataProcessor::DataType &selectedDataType, const DataProcessor::DataType &filterType)
{
	
	DBG("Select :: filter Type = " + String(filterType));
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

		DBG("Is data compatible ? "+ String(d->isTypeCompatible(filterType)));

		menu->addItem(itemID,d->name + " ("+d->getTypeString()+")", d->isTypeCompatible(filterType));

		for (int j = 0; j < d->elements.size(); j++)
		{
			menu->addItem(itemID + (j + 1), ".    | "+d->elements[j]->name, d->elements[j]->isTypeCompatible(filterType));
			//dataMenu->addItem(itemID + (j + 1), d->elements[j]->name,d->elements[j]->isTypeCompatible(filterType));
			//DBG(" > Is element compatible ? " + String(d->elements[j]->isTypeCompatible(filterType)));

		}
		
		//menu->addSubMenu(d->name, *dataMenu, true, nullptr, false, d->isTypeCompatible(filterType)?itemID:0);
	}

	int resultID = menu->show();

	if (resultID > 0)
	{
		int offsetID = resultID - 1;

		int dataID = (int)floor(offsetID / 10);
		int elementID = offsetID % maxElementPerData;

		selectedDataName = datas->getUnchecked(dataID)->name;
		if (elementID > 0)
		{
			selectedElementName = datas->getUnchecked(dataID)->elements[elementID - 1]->name;
			selectedDataType = datas->getUnchecked(dataID)->elements[elementID - 1]->type;
		}
		else
		{
			selectedDataType = datas->getUnchecked(dataID)->type;
		}
	}
	
}

NodeManagerUI * ConnectorComponent::getNodeManagerUI() const noexcept
{
	return findParentComponentOfClass<NodeManagerUI>();
}

NodeBaseUI * ConnectorComponent::getNodeUI() const noexcept
{
	return findParentComponentOfClass<NodeBaseUI>();
}
