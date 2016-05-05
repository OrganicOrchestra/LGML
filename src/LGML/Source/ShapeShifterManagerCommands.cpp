#include "ShapeShifterManager.h"

StringArray ShapeShifterManager::getMenuBarNames()
{
	StringArray sa;
	sa.add("Panels");
	sa.add("Layout");
	return sa;
}

PopupMenu ShapeShifterManager::getMenuForIndex(int topLevelMenuIndex, const String &)
{
	PopupMenu menu;
#if __cplusplus < 201103L
    Array<String> tmpNames;
    tmpNames.add( "Panel1");tmpNames.add( "Panel2");tmpNames.add( "Panel3");
#else
	Array<String> tmpNames = { "Panel1","Panel2","Panel3" };
#endif

	if (topLevelMenuIndex == 0) // "Panels" menu
	{
		int startID = 1;
		for (auto &s : tmpNames)
		{		
			menu.addItem(startID, s);
			startID++;
		}
		
	}if (topLevelMenuIndex == 1) // "Layout" menu
	{
		menu.addItem(1,"Revert to default Layout");
	}
	
	return menu;
}

void ShapeShifterManager::menuItemSelected(int menuItemID, int )
{
	DBG("Item selected : " << menuItemID);
}