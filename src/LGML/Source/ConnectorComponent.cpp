#include "NodeManagerUI.h"
#include "ConnectorComponent.h"

ConnectorComponent::ConnectorComponent(ConnectorIOType ioType, NodeConnection::ConnectionType dataType, NodeBase * node) :
    ioType(ioType), dataType(dataType), node(node),isHovered(false)
{
    boxColor = dataType == NodeConnection::ConnectionType::AUDIO ? AUDIO_COLOR : DATA_COLOR;
    setSize(10,10);


    String tooltip;
    tooltip += dataType == NodeConnection::ConnectionType::AUDIO?"Audio\n":"Data\n";
    if (dataType == NodeConnection::ConnectionType::AUDIO)
    {
        tooltip += ioType == ConnectorIOType::INPUT? node->audioProcessor->getTotalNumInputChannels() : node->audioProcessor->getTotalNumOutputChannels();
        tooltip += " channels";
    }
    else
    {
        StringArray dataInfos = ioType == ConnectorIOType::INPUT ? node->dataProcessor->getInputDataInfos() : node->dataProcessor->getOutputDataInfos();
        tooltip += dataInfos.joinIntoString("\n");
    }

    setTooltip(tooltip);

}

void ConnectorComponent::paint(Graphics & g)
{
    g.setGradientFill(ColourGradient(isHovered?boxColor.brighter(5.f):boxColor, (float)(getLocalBounds().getCentreY()),(float)(getLocalBounds().getCentreY()), boxColor.darker(), 0.f,0.f, true));
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 2);
}

void ConnectorComponent::mouseDown(const MouseEvent &)
{
    NodeManagerUI * nmui = getNodeManagerUI();

    if (dataType == NodeConnection::ConnectionType::DATA)
    {

        nmui->createDataConnectionFromConnector(this);
    }
    else
    {
        nmui->createAudioConnectionFromConnector(this);
    }
}

void ConnectorComponent::mouseEnter (const MouseEvent&){
    isHovered = true;
    repaint();
}
void ConnectorComponent::mouseExit  (const MouseEvent&){
    isHovered = false;
    repaint();
}

/*
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


    ScopedPointer<PopupMenu> menu (new PopupMenu());

    const int maxElementPerData = 10;

    for (int i = 0; i < numDatas; i++)
    {
        ScopedPointer<PopupMenu> dataMenu( new PopupMenu());
        DataProcessor::Data * d = datas->getUnchecked(i);
        int itemID = i *  maxElementPerData + 1; //max 10 element per data anyway, 1 to not start from 0

        DBG("Is data compatible ? "+ String(d->isTypeCompatible(filterType)));

        menu->addItem(itemID,d->name + " ("+d->getTypeString()+")", d->isTypeCompatible(filterType));

        for (int j = 0; j < d->elements.size(); j++)
        {
            menu->addItem(itemID + (j + 1), ".    | "+d->elements[j]->name, d->elements[j]->isTypeCompatible(filterType));
            //dataMenu->addItem(itemID + (j + 1), d->elements[j]->name,d->elements[j]->isTypeCompatible(filterType));
            //DBG(" > Is element compatible ? " + String(d->elements[j]->isTypeCompatible(filterType)))
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
*/

NodeManagerUI * ConnectorComponent::getNodeManagerUI() const noexcept
{
    return findParentComponentOfClass<NodeManagerUI>();
}

NodeBaseUI * ConnectorComponent::getNodeUI() const noexcept
{
    return findParentComponentOfClass<NodeBaseUI>();
}
