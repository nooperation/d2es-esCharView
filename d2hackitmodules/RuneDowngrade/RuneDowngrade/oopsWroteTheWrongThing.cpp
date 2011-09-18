//#include "RuneDowngrade.h"
//
//#include <vector>
//#include <string>
//
//BOOL CALLBACK EnumCubeItems(LPCITEM lpItem, LPARAM lParam)
//{
//	((std::vector<LPCITEM> *)lParam)->push_back(lpItem);
//	return true;
//};
//
//RuneDowngrade::RuneDowngrade()
//{
//	m_currentState = STATE_Inactive;
//	m_isExtractorInCube = false;
//	m_isStockerInCube = false;
//
//	//stockers.insert("s01"); // Rune Stocker Low
//	stockers.insert("s02"); // Rune Stocker Low
//	stockers.insert("s03"); // Rune Stocker Low
//	stockers.insert("s04"); // Rune Stocker Low
//	stockers.insert("s05"); // Rune Stocker Low
//	stockers.insert("s06"); // Rune Stocker Low
//	stockers.insert("s07"); // Rune Stocker Low
//	stockers.insert("s08"); // Rune Stocker Low
//	stockers.insert("s09"); // Rune Stocker Low
//	stockers.insert("s10"); // Rune Stocker Low
//	stockers.insert("s11"); // Rune Stocker Low
//	stockers.insert("s12"); // Rune Stocker Low
//	stockers.insert("s13"); // Rune Stocker Low
//	stockers.insert("s14"); // Rune Stocker Low
//	stockers.insert("s15"); // Rune Stocker Low
//	stockers.insert("s16"); // Rune Stocker Low
//	stockers.insert("s17"); // Rune Stocker Low
//	stockers.insert("s18"); // Rune Stocker Low
//	stockers.insert("s19"); // Rune Stocker Low
//	stockers.insert("s20"); // Rune Stocker Low
//	stockers.insert("s21"); // Rune Stocker Low
//
//	runes.insert("r01"); // �c/I Rune
//	runes.insert("r02"); // �c/U Rune
//	runes.insert("r03"); // �c/Shi Rune
//	runes.insert("r04"); // �c/Ka Rune
//	runes.insert("r05"); // �c/N Rune
//	runes.insert("r06"); // �c/Ku Rune
//	runes.insert("r07"); // �c/Yo Rune
//	runes.insert("r08"); // �c1Ki Rune
//	runes.insert("r09"); // �c1Ri Rune
//	runes.insert("r10"); // �c1Mi Rune
//	runes.insert("r11"); // �c1Ya Rune
//	runes.insert("r12"); // �c1A Rune
//	runes.insert("r14"); // �c1Chi Rune
//	runes.insert("r15"); // �c9Sa Rune
//	runes.insert("r17"); // �c9Ke Rune
//	runes.insert("r18"); // �c9E Rune
//	runes.insert("r19"); // �c9Ko Rune
//	runes.insert("r20"); // �c9Ra Rune
//	runes.insert("r21"); // �c9O Rune
//}
//
//bool RuneDowngrade::CheckCubeForKeyAndStocker()
//{
//	bool hasKey = false;
//	bool hasStocker = false;
//	std::vector<LPCITEM> itemsInCube;
//
//	me->EnumStorageItems(STORAGE_CUBE, EnumCubeItems, (LPARAM)(&itemsInCube));
//
//	if(itemsInCube.size() != 2)
//	{
//		return false;
//	}
//
//	for each(auto item in itemsInCube)
//	{
//		if(stockers.count(item->szItemCode) != 0)
//		{
//			hasStocker = true;
//		}
//		else if(_stricmp(item->szItemCode, "key") == 0)
//		{
//			hasKey = true;
//		}
//	}
//	
//	return hasStocker && hasKey;
//}
//
//void RuneDowngrade::Start(int count)
//{
//	if(count == 0)
//	{
//		server->GameStringf("Complete");
//		m_currentState = STATE_Inactive;
//		return;
//	}
//
//	m_count = count;
//	ExtractRune();
//}
//
//void RuneDowngrade::ExtractRune()
//{
//	if(!me->IsUIOpened(UI_CUBE))
//	{
//		me->OpenCube();
//	}
//
//	if(!CheckCubeForKeyAndStocker())
//	{
//		server->GameStringf("RuneDowngrade: Cube must contain key and low rune stocker");
//		m_currentState = STATE_Inactive;
//		return;
//	}
//	
//	m_currentState = STATE_WaitingForItemToCube;
//	m_isExtractorInCube = false;
//	m_isStockerInCube = false;
//	m_extractedItemID = -1;
//
//	me->Transmute();
//}
//
///// <summary>
///// Called whenever an item is moved to the cube
///// </summary>
///// <param name="item">The item that was moved to the cube.</param>
//void RuneDowngrade::OnItemToCube(const ITEM &item)
//{
//	if(m_currentState != STATE_WaitingForItemToCube)
//	{
//		return;
//	}
//
//	if(stockers.count(item.szItemCode) != 0)
//	{
//		m_isStockerInCube = true;
//		return;
//	}
//	else if(_stricmp(item.szItemCode, "key") == 0)
//	{
//		m_isExtractorInCube = true;
//		return;
//	}
//	else if(runes.count(item.szItemCode) == 0)
//	{
//		server->GameStringf("Invalid item in cube!");
//		m_currentState = STATE_Inactive;
//		return;
//	}
//	else
//	{
//		m_extractedItemID = item.dwItemID;
//	}
//
//	// All items have been extracted, begin moving rune to the cube
//	if(m_isStockerInCube && m_isExtractorInCube && m_extractedItemID != -1)
//	{
//		m_currentState = STATE_WaitingForItemFromCube;
//		if(!me->PickStorageItemToCursor(m_extractedItemID))
//		{
//			server->GameStringf("Failed to pickup extracted item!");
//			m_currentState = STATE_Inactive;
//			return;
//		}
//	}		
//}
//
///// <summary>
///// Called whenever an item is pickedup to the cursor from the cube
///// </summary>
///// <param name="item">Item picked up from the cube.</param>
//void RuneDowngrade::OnItemFromCube(const ITEM &item)
//{
//	if(m_currentState != STATE_WaitingForItemFromCube)
//	{
//		return;
//	}
//
//	m_currentState = STATE_WaitingForItemToInventory;
//	if(!me->DropCursorItemToStorage(STORAGE_INVENTORY))
//	{
//		server->GameStringf("Failed to drop extracted item to inventory!");
//		m_currentState = STATE_Inactive;
//		return;
//	}
//}
//
///// <summary>
///// Called whenever an item is moved to the player's inventory
///// </summary>
///// <param name="item">The item that was moved to the player's inventory.</param>
//void RuneDowngrade::OnItemToInventory(const ITEM &item)
//{
//	if(m_currentState != STATE_WaitingForItemToInventory)
//	{
//		return;
//	}
//
//	m_currentState = STATE_Inactive;
//	Start(m_count - 1);
//}