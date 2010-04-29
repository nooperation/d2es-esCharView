#include "../../d2hackit/includes/D2Client.h"
#include "AutoReroll.h"
#include <fstream>
#include <string>

#include "../../d2hackit/includes/itemPrefix.h"
#include "../../d2hackit/includes/itemSuffix.h"

stdext::hash_set<std::string> gemItemCodes;

AutoReroll::AutoReroll()
{
	currentState = STATE_UNINITIALIZED;

	gemItemCodes.clear();
	gemItemCodes.insert("gcv"); // �c;Chipped Amethyst
	gemItemCodes.insert("gfv"); // �c;Flawed Amethyst
	gemItemCodes.insert("gsv"); // �c;Amethyst
	gemItemCodes.insert("gzv"); // �c;Flawless Amethyst
	gemItemCodes.insert("gpv"); // �c;Blemished Amethyst
	gemItemCodes.insert("gcy"); // �c9Chipped Topaz
	gemItemCodes.insert("gfy"); // �c9Flawed Topaz
	gemItemCodes.insert("gsy"); // �c9Topaz
	gemItemCodes.insert("gly"); // �c9Flawless Topaz
	gemItemCodes.insert("gpy"); // �c9Blemished Topaz
	gemItemCodes.insert("gcb"); // �c3Chipped Sapphire
	gemItemCodes.insert("gfb"); // �c3Flawed Sapphire
	gemItemCodes.insert("gsb"); // �c3Sapphire
	gemItemCodes.insert("glb"); // �c3Flawless Sapphire
	gemItemCodes.insert("gpb"); // �c3Blemished Sapphire
	gemItemCodes.insert("gcg"); // �c2Chipped Emerald
	gemItemCodes.insert("gfg"); // �c2Flawed Emerald
	gemItemCodes.insert("gsg"); // �c2Emerald
	gemItemCodes.insert("glg"); // �c2Flawless Emerald
	gemItemCodes.insert("gpg"); // �c2Blemished Emerald
	gemItemCodes.insert("gcr"); // �c1Chipped Ruby
	gemItemCodes.insert("gfr"); // �c1Flawed Ruby
	gemItemCodes.insert("gsr"); // �c1Ruby
	gemItemCodes.insert("glr"); // �c1Flawless Ruby
	gemItemCodes.insert("gpr"); // �c1Blemished Ruby
	gemItemCodes.insert("gcw"); // �c/Chipped Diamond
	gemItemCodes.insert("gfw"); // �c/Flawed Diamond
	gemItemCodes.insert("gsw"); // �c/Diamond
	gemItemCodes.insert("glw"); // �c/Flawless Diamond
	gemItemCodes.insert("gpw"); // �c/Blemished Diamond
	gemItemCodes.insert("skc"); // �c0Chipped Skull
	gemItemCodes.insert("skf"); // �c0Flawed Skull
	gemItemCodes.insert("sku"); // �c0Skull
	gemItemCodes.insert("skl"); // �c0Flawless Skull
	gemItemCodes.insert("skz"); // �c0Blemished Skull
	gemItemCodes.insert("gvb"); // �c;Perfect Amethyst
	gemItemCodes.insert("gyb"); // �c9Perfect Topaz
	gemItemCodes.insert("gbb"); // �c3Perfect Sapphire
	gemItemCodes.insert("ggb"); // �c2Perfect Emerald
	gemItemCodes.insert("grb"); // �c1Perfect Ruby
	gemItemCodes.insert("gwb"); // �c/Perfect Diamond
	gemItemCodes.insert("skb"); // �c0Perfect Skull
	gemItemCodes.insert("gck"); // �c5Chipped Obsidian
	gemItemCodes.insert("gfk"); // �c5Flawed Obsidian
	gemItemCodes.insert("gsk"); // �c5Obsidian
	gemItemCodes.insert("gzk"); // �c5Flawless Obsidian
	gemItemCodes.insert("gpk"); // �c5Blemished Obsidian
	gemItemCodes.insert("gbk"); // �c5Perfect Obsidian
}

bool AutoReroll::Init(bool useChat)
{
	this->useChat = useChat;

	if(!ReadConfig(".\\plugin\\goodPrefix_ar.txt", goodPrefix))
		return false;

	if(!ReadConfig(".\\plugin\\goodSuffix_ar.txt", goodSuffix))
		return false;

	minPrefix = GetPrivateProfileInt("AutoReroll", "PrefixCount", 2, CONFIG_PATH);
	minSuffix = GetPrivateProfileInt("AutoReroll", "SuffixCount", 0, CONFIG_PATH);
	numGemsToUse = GetPrivateProfileInt("AutoReroll", "GemCount", 2, CONFIG_PATH);

	server->GameStringf("Min prefix: %d Min suffix: %d", minPrefix, minSuffix);

	extractedItemID = 0;
	justRanAutoExtractor = false;
	isExtractedItemGood = false;
	loadedEmptyCube = false;

	return true;
}

bool AutoReroll::ReadConfig(std::string configPath, stdext::hash_set<int> &readTo)
{
	std::string readLineBuff;
	int readNum = 0;

	readTo.clear();

	std::ifstream inFile(configPath.c_str());
	if(!inFile)
	{
		server->GameStringf("�c:AutoReroll�c0: Failed to open file %s\n", configPath.c_str());
		return false;
	}

	while(inFile.good())
	{
		std::getline(inFile, readLineBuff);

		readNum = atoi(readLineBuff.c_str());
		if(readNum > 0)
		{
			readTo.insert(readNum);
		}
	}

	inFile.close();

	return true;
}

bool AutoReroll::Start(int numGems, bool useChat)
{
	if(!Init(useChat))
	{
		return false;
	}

	if(numGems > 0)
	{
		server->GameStringf("�c:AutoReroll�c0: Using %d gems", numGems);
		numGemsToUse = numGems;
	}

	return StartExtraction();
}

bool AutoReroll::CheckExtractedItem(const ITEM &item)
{
	int goodPrefixCount = 0;
	int goodSuffixCount = 0;

	if(item.iQuality == ITEM_QUALITY_RARE || item.iQuality == ITEM_QUALITY_MAGIC || item.iQuality == ITEM_QUALITY_CRAFT)
	{
		for(int i = 0; i < ITEM_MAX_PREFIX; i++)
		{
			if(goodPrefix.count(item.wPrefix[i]) > 0)
			{
				goodPrefixCount++;
				server->GameStringf("�c;Prefix�c0 %s: %s", Prefix[item.wPrefix[i]], PrefixDetails[item.wPrefix[i]]);
			}
			if(goodSuffix.count(item.wSuffix[i]) > 0)
			{
				goodSuffixCount++;
				server->GameStringf("�c:Suffix�c0 %s: %s", Suffix[item.wSuffix[i]], SuffixDetails[item.wSuffix[i]]);
			}
		}
	}

	return goodPrefixCount >= minPrefix && goodSuffixCount >= minSuffix;
}

bool AutoReroll::StartExtraction()
{
	int itemCount = 0;

	if(!me->OpenCube())
	{
		if(useChat)
			me->Say("�c:AutoReroll�c0: Cube not opened");

		server->GamePrintString("�c:AutoReroll�c0: Cube not opened");
		currentState = STATE_COMPLETE;
		return false;
	}

	me->EnumStorageItems(STORAGE_CUBE, enumItemCountProc, (LPARAM)&itemCount);
	if(itemCount != 1 && extractedItemID == 0)
	{
		if(useChat)
			me->Say("�c:AutoReroll�c0: Place only one rare weapon/armor to reroll in cube before starting");
		
		server->GamePrintString("�c:AutoReroll�c0: Place only one rare weapon/armor to reroll in cube before starting");
		currentState = STATE_COMPLETE;
		return false;
	}
	else if(itemCount == 1 && extractedItemID == 0)
	{
		me->EnumStorageItems(STORAGE_CUBE, enumFindItemToReroll, (LPARAM)&extractedItemID);
	}

	currentExtractor = 0;
	extractors.clear();
	me->EnumStorageItems(STORAGE_INVENTORY, enumFindGems, (LPARAM)&extractors);

	if(extractors.size() <= 1)
	{
		if(justRanAutoExtractor)
		{
			if(useChat)
				me->Say("�c:AutoReroll�c0: No more gems");
			else
				server->GamePrintString("�c:AutoReroll�c0: No more gems");

			currentState = STATE_COMPLETE;
			return true;
		}
		else
		{
			currentState = STATE_RUNNINGEMPTYCUBE;
			server->GameCommandLine("load emptycube");
			server->GameCommandLine("emptycube start chat");
			loadedEmptyCube = true;
			return true;
		}
	}

	if(justRanAutoExtractor)
	{
		justRanAutoExtractor = false;
		if(extractedItemID != 0)
		{
			currentState = STATE_PICKUPEXTRACTEDITEM;
			if(!me->PickStorageItemToCursor(extractedItemID))
			{
				if(useChat)
					me->Say("�c:AutoReroll�c0: Unable to pickup previously extracted item from inventory");

				server->GamePrintString("�c:AutoReroll�c0: Unable to pickup previously extracted item from inventory");
				currentState = STATE_COMPLETE;
				return false;
			}
		}
	}
	else
	{
		justRanAutoExtractor = false;
		currentState = STATE_NEXTEXTRACTOR;
	}
	return true;
}

void AutoReroll::OnTick()
{
	switch(currentState)
	{
		case STATE_NEXTGEMCANSTUFF:
		{
			currentState = STATE_PICKUPGEMCANSTUFF;
			DWORD itemToPickup = 0;

			if(!gemCanAndOpener.GemCanMoved)
			{
				gemCanAndOpener.GemCanMoved = true;
				itemToPickup = gemCanAndOpener.GemCanID;
			}
			else if(!gemCanAndOpener.CanOpenerMoved)
			{
				gemCanAndOpener.CanOpenerMoved = true;
				itemToPickup = gemCanAndOpener.CanOpenerID;
			}

			if(itemToPickup != 0)
			{
				if(!me->PickStorageItemToCursor(itemToPickup))
				{
					if(useChat)
						me->Say("�c:AutoReroll�c0: Failed drop pickup can opener");
						
					server->GamePrintString("�c:AutoReroll�c0: Failed drop pickup  can opener");

					currentState = STATE_COMPLETE;
					return;
				}
			}
			break;
		}
		case STATE_NEXTEXTRACTOR:
		{
			currentState = STATE_WAITINGFORNEXTSTATE;

			if(currentExtractor >= (int)extractors.size())
			{
				if(useChat)
					me->Say("�c:AutoReroll�c0: currentExtractor > extractors.size()");

				server->GamePrintString("�c:AutoReroll�c0: currentExtractor > extractors.size()");
				currentState = STATE_COMPLETE;
				return;
			}

			currentState = STATE_PICKUPEXTRACTOR;
			if(!me->PickStorageItemToCursor(extractors[currentExtractor]))
			{
				if(useChat)
					me->Say("�c:AutoReroll�c0: Failed to pickup extractor");

				server->GamePrintString("�c:AutoReroll�c0: Failed to pickup extractor");
				currentState = STATE_COMPLETE;
				return;
			}

			break;
		}
		case STATE_STARTEXTRACTION:
		{
			currentState = STATE_TRANSMUTE;
			me->Transmute();
			break;
		}
		case STATE_EXTRACTIONCOMPLETE:
		{
			currentState = STATE_COMPLETE;
			break;
		}
		case STATE_TRANSMUTE_COMPLETE:
		{
			currentState = STATE_WAITINGFORNEXTSTATE;

			if(!isExtractedItemGood)
			{
				//server->GameStringf("Junk, trying next");
				StartExtraction();
				return;
			}
			else
			{
				if(useChat)
					me->Say("�c:AutoReroll�c0: Rolled good item");
				else
					server->GamePrintString("�c:AutoReroll�c0: Rolled good item");

				currentState = STATE_COMPLETE;
				return;
			}

			break;
		}
		case STATE_COMPLETE:
		{
			currentState = STATE_UNINITIALIZED;

			if(loadedEmptyCube)
				server->GameCommandLine("unload emptycube");

			if(useChat)
				me->Say("�c:AutoReroll�c0: AutoReroll Ended");
			else
				server->GamePrintString("�c:AutoReroll�c0: AutoReroll Ended");

			break;
		}
		case STATE_FINISHEDEMPTYCUBE:
		{
			currentState = STATE_WAITINGFORNEXTSTATE;

			// 1 - Finished AutoExtractor, ran EmptyCube - we have gems and are ready to restart
			if(justRanAutoExtractor)
			{
				StartExtraction();
				return;
			}
			else
			{
				// 2 - Ready to move GemCan + CanOpener to cube and run AutoExtractor
				me->EnumStorageItems(STORAGE_INVENTORY, enumFindGemCanStuff, (LPARAM)&gemCanAndOpener);
				if(gemCanAndOpener.CanOpenerID == 0 || gemCanAndOpener.GemCanID == 0)
				{
					if(useChat)
						me->Say("�c:AutoReroll�c0: Unable to find gem can or gem can opener");

					server->GameStringf("�c:AutoReroll�c0: Unable to find %s%s", (gemCanAndOpener.CanOpenerID==0)?" Can Opener":"", (gemCanAndOpener.GemCanID==0)?" Gem Gan":"");

					currentState = STATE_COMPLETE;
					return;
				}

				currentState = STATE_NEXTGEMCANSTUFF;
			}
		}
	}
}

void AutoReroll::OnItemFromInventory(DWORD itemID)
{
	switch(currentState)
	{
		case STATE_PICKUPGEMCANSTUFF:
		{
			currentState = STATE_GEMCANSTUFFTOCUBE;
			break;
		}
		case STATE_PICKUPEXTRACTOR:
		{
			currentState = STATE_EXTRACTORTOCUBE;
			break;
		}
		case STATE_PICKUPEXTRACTEDITEM:
		{
			currentState = STATE_EXTRACTEDTOCUBE;
			break;
		}
		default:
		{
			return;
		}
	}

	if(!me->DropCursorItemToStorage(STORAGE_CUBE))
	{
		if(useChat)
			me->Say("�c:AutoReroll�c0: Failed drop inventory item to cube");
			
		server->GameStringf("�c:AutoReroll�c0: Failed drop inventory item to cube");

		currentState = STATE_COMPLETE;
		return;
	}
}

void AutoReroll::OnItemToCube(const ITEM &item)
{
	switch(currentState)
	{
		case STATE_TRANSMUTE:
		{
			// Only thing to cube will be extracted item
			extractedItemID = item.dwItemID;
			isExtractedItemGood = CheckExtractedItem(item);

			currentState = STATE_TRANSMUTE_COMPLETE;
			break;
		}
		case STATE_EXTRACTORTOCUBE: // The flawless gem
		{
			currentExtractor++;
			if(currentExtractor >= numGemsToUse)
			{
				currentState = STATE_STARTEXTRACTION;
			}
			else
			{
				currentState = STATE_NEXTEXTRACTOR;
			}
			break;
		}
		case STATE_EXTRACTEDTOCUBE: // The item we're rerolling
		{
			currentState = STATE_NEXTEXTRACTOR;
			break;
		}
		case STATE_GEMCANSTUFFTOCUBE: // Gem can and can opener
		{
			if(gemCanAndOpener.CanOpenerMoved && gemCanAndOpener.GemCanMoved)
			{
				currentState = STATE_RUNNINGAUTOEXTRACTOR;
				server->GameCommandLine("load ae");
				server->GameCommandLine("ae start chat");
			}
			else
			{
				currentState = STATE_NEXTGEMCANSTUFF;
			}
			break;
		}
	}
}
bool AutoReroll::OnAutoExtractorEnded()
{
	if(currentState != STATE_RUNNINGAUTOEXTRACTOR)
		return false;

	// Clean cube again!
	justRanAutoExtractor = true;
	currentState = STATE_RUNNINGEMPTYCUBE;
	server->GameCommandLine("emptycube start chat");

	return true;
}

bool AutoReroll::OnEmptyCubeEnded()
{
	if(currentState != STATE_RUNNINGEMPTYCUBE)
		return false;

	int itemCount = 0;
	memset(&gemCanAndOpener, 0, sizeof(GemCanStuff));

	me->EnumStorageItems(STORAGE_CUBE, enumItemCountProc, (LPARAM)&itemCount);
	if(itemCount != 0)
	{
		server->GameStringf("�c:AutoReroll�c0: Unable to clean cube");
		currentState = STATE_COMPLETE;
	}
	else
	{
		currentState = STATE_FINISHEDEMPTYCUBE;
	}

	return true;
}

void AutoReroll::Abort()
{
	if(currentState != STATE_COMPLETE && currentState != STATE_UNINITIALIZED)
		currentState = STATE_COMPLETE;
}

bool IsGem(const char *itemCode)
{
	if(itemCode[0] == 'g')
	{
		if(itemCode[1] == 'l')
		{
			return (itemCode[2] == 'y' ||itemCode[2] == 'b' ||itemCode[2] == 'g' ||itemCode[2] == 'r' ||itemCode[2] == 'w');
		}
		else if(itemCode[1] == 'z')
		{
			return (itemCode[2] == 'k' || itemCode[2] == 'v');
		}
	}

	return (_stricmp(itemCode, "skl") == 0);
}

BOOL CALLBACK enumItemCountProc(LPCITEM item, LPARAM lParam)
{
	(*(int *)lParam)++;
	return TRUE;
}

BOOL CALLBACK enumFindItemToReroll(LPCITEM item, LPARAM lParam)
{
	(*(DWORD *)lParam) = item->dwItemID;
	return TRUE;
}

BOOL CALLBACK enumFindGemCanStuff(LPCITEM item, LPARAM lParam)
{
	GemCanStuff *canStuff = (GemCanStuff *)lParam;

	if(item->szItemCode[0] == 'k' && isdigit(item->szItemCode[2]))
	{
		// k[^o]0 is gem can
		if(item->szItemCode[1] != 'o' && item->szItemCode[2] == '0')
		{
			if(item->szItemCode[1] == 'v' || item->szItemCode[1] == 'y' ||
				item->szItemCode[1] == 'b' || item->szItemCode[1] == 'g' ||
				item->szItemCode[1] == 'r' || item->szItemCode[1] == 'w' ||
				item->szItemCode[1] == 's' || item->szItemCode[1] == 'k')
			{
				canStuff->GemCanID = item->dwItemID;

				return canStuff->CanOpenerID == 0;
			}
		}
		else // ko[0-5] is can opener
		{
			int itemCodeNum = atoi(item->szItemCode+2);
			if(itemCodeNum >= 0 && itemCodeNum <= 5)
			{
				canStuff->CanOpenerID = item->dwItemID;

				return canStuff->GemCanID == 0;
			}
		}
	}

	return TRUE;
}

BOOL CALLBACK enumFindGems(LPCITEM item, LPARAM lParam)
{
	if(gemItemCodes.count(item->szItemCode) > 0)
	{
		((std::vector<DWORD>*)lParam)->push_back(item->dwItemID);
	}

	return TRUE;
}