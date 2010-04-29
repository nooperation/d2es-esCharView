#include "../../d2hackit/includes/ClientCore.cpp"
#include "AutoReroll.h"

AutoReroll autoReroll;

int ticksSkipped = 0;
int speed = 0;
bool autoRerollStarted;

CLIENTINFO
(
	0,1,
	"",
	"",
	"Auto Reroll",
	""
)

BOOL PRIVATE Start(char** argv, int argc)
{
	bool useChat = false;
	int numGems = 0;

	if(argc >= 3)
	{
		for(int i = 2; i < argc; i++)
		{
			if(_stricmp(argv[i], "chat") == 0)
				useChat = true;
			else if(isdigit(argv[i][0]))
			{
				int tempNumGems = atoi(argv[i]);

				if(tempNumGems >= 0 && tempNumGems < 7)
				{
					numGems = tempNumGems;
				}
			}
		}
	}

	autoRerollStarted = true;
	autoReroll.Start(numGems, useChat);

	return TRUE;
}


BOOL PRIVATE Speed(char** argv, int argc)
{
	if(argc == 3)
	{
		speed = atoi(argv[2]);

		if(speed < 0)
			speed = 0;

		server->GameStringf("Setting speed to %d", speed);
		return TRUE;
	}

	return FALSE;
}

DWORD EXPORT OnGameTimerTick()
{
	if(ticksSkipped++ < speed)
		return 0;

	autoReroll.OnTick();
	ticksSkipped = 0;

	return 0;
}

DWORD EXPORT OnGamePacketBeforeSent(BYTE* aPacket, DWORD aLen)
{
	if(aPacket[0] == 0x15 && aPacket[1] == 0x01)
	{
		char *chatMessage = (char *)(aPacket+3);

		if(strncmp(chatMessage, "�c5EmptyCube�c0:", 16) == 0)
		{
			if(strcmp(chatMessage, "�c5EmptyCube�c0: EmptyCube Ended") == 0)
			{
				if(!autoReroll.OnEmptyCubeEnded())
				{
					return aLen;
				}
				else
				{
					return 0;
				}
			}
			else
			{
				return 0;
			}
		}
		else if(strncmp(chatMessage, "�c:AutoExtractor�c0:", 20) == 0)
		{
			if(strcmp(chatMessage, "�c:AutoExtractor�c0: AutoExtractor Ended") == 0)
			{
				if(!autoReroll.OnAutoExtractorEnded())
				{
					return aLen;
				}
				else
				{
					return 0;
				}
			}
			else
			{
				return 0;
			}
		}
	}

	return aLen;
}

VOID EXPORT OnThisPlayerMessage(UINT nMessage, WPARAM wParam, LPARAM lParam)
{
	if(nMessage == PM_UICLOSED)
	{
		autoReroll.Abort();
	}
}

VOID EXPORT OnUnitMessage(UINT nMessage, LPCGAMEUNIT lpUnit, WPARAM wParam, LPARAM lParam)
{
	if(nMessage == UM_ITEMEVENT)
	{
		ITEM item = *(ITEM *)lParam;

		if(wParam == 0x04 && item.iStorageID == 0x04)
		{
			autoReroll.OnItemToCube(item);
		}
		//else if(wParam == 0x04 && item.iStorageID == 0x01)
		//{
		//	autoReroll.OnItemToInventory(item);
		//}
		else if(wParam == 0x05 && item.iStorageID == 0x01)
		{
			autoReroll.OnItemFromInventory(item.dwItemID);
		}
		//else if(wParam == 0x05 && item.iStorageID == 0x04)
		//{
		//	autoReroll.OnItemFromCube(item.dwItemID);
		//}
	}
}

BYTE EXPORT OnGameKeyDown(BYTE iKeyCode)
{
	switch(iKeyCode)
	{
		case VK_SPACE:
			autoReroll.Abort();
			break;
	}

	return iKeyCode;
}

MODULECOMMANDSTRUCT ModuleCommands[]=
{
	{
		"help",
		OnGameCommandHelp,
		"help�c0 List commands available in this module.\n"
		"<command> help�c0 Shows detailed help for <command> in this module."
	},
	{
		"Start",
		Start,
		"Usage: Start [chat]",
	},
	{
		"Speed",
		Speed,
		"Speed # where # is the number of ticks to wait per command. Each tick is 100ms"
	},
	{NULL}
};


