#include "../../d2hackit/includes/ClientCore.cpp"
#include "ExpWatcher.h"

BOOL PRIVATE Show(char** argv, int argc);
BOOL PRIVATE Hide(char** argv, int argc);
BOOL PRIVATE TellStats(char** argv, int argc);
BOOL PRIVATE Reset(char** argv, int argc);

ExpWatcher expWatcher;

BOOL PRIVATE ShowExp(char** argv, int argc)
{
	bool useVerbose = false;

	if(argc == 3)
	{
		useVerbose = (atoi(argv[2]) != 0);
	}

	expWatcher.EnableWatcher(useVerbose);
	expWatcher.PrintExpInfo();
	return TRUE;
}

BOOL PRIVATE HideExp(char** argv, int argc)
{
	expWatcher.DisableWatcher();
	server->GamePrintInfo("No longer showing exp info");

	return TRUE;
}

BOOL PRIVATE TellStats(char** argv, int argc)
{
	char statsText[1024];

	sprintf_s(statsText, "�c0Str: �c8%d �c0Dex: �c8%d �c0Vit: �c8%d �c0Eng: �c8%d �c0Hp: �c8%d �c0Mp: �c8%d �c0Res: �c1%d�c0/�c9%d�c0/�c3%d�c0/�c2%d�c0", 
		me->GetStat(STAT_STRENGTH),
		me->GetStat(STAT_DEXTERITY), 
		me->GetStat(STAT_VITALITY),
		me->GetStat(STAT_ENERGY),
		me->GetStat(STAT_MAXHP),
		me->GetStat(STAT_MAXMANA),
		me->GetStat(STAT_FIRERESIST),
		me->GetStat(STAT_LIGHTNINGRESIST),
		me->GetStat(STAT_COLDRESIST),
		me->GetStat(STAT_POISONRESIST));

	me->Say(statsText);

	return TRUE;
}

BOOL PRIVATE ResetExp(char** argv, int argc)
{
	expWatcher.ResetAverageExp();
	server->GamePrintInfo("Exp average reset");
	return TRUE;
}


////////////////////////////////////////////
//
//               EXPORTS
//
/////////////////////////////////////////////


CLIENTINFO
(
	0,1,
	"",
	"",
	"Exp",
	""
)

MODULECOMMANDSTRUCT ModuleCommands[]=
{
	{
		"help",
		OnGameCommandHelp,
		"help�c0 List commands available in this module.\n"
		"<command> help�c0 Shows detailed help for <command> in this module."
	},
	{
		"Show",
		ShowExp,
		"Shows exp on screen, ShowExp [1/0] for verbose"
	},
	{
		"Hide",
		HideExp,
		"Hides exp on screen"
	},
	{
		"Reset",
		ResetExp,
		"Resets experience average"
	},
	{NULL}
};



VOID EXPORT OnGameJoin(THISGAMESTRUCT* thisgame)
{
	expWatcher.ResetAverageExp();
	expWatcher.StartZoneExp();
	expWatcher.StartGameExp();
}

VOID EXPORT OnUnitMessage(UINT nMessage, LPCGAMEUNIT lpUnit, WPARAM wParam, LPARAM lParam)
{
	if(nMessage == UM_DEATH)
	{
		expWatcher.CheckForExp();
	}
}

VOID EXPORT OnThisPlayerMessage(UINT nMessage, WPARAM wParam, LPARAM lParam)
{
	if(nMessage == PM_ENTERTOWN || nMessage == PM_MAPCHANGE)
	{
		if(expWatcher.GetTotalZoneExp() > 0)
		{
			expWatcher.PrintExpInfo();
		}

		expWatcher.ResetAverageExp();
		expWatcher.StartZoneExp();
	}
	if(nMessage == PM_LEAVETOWN)
	{
		// This is just to initilize the exp values since
		//   they will think we have 0 exp until checked.
		//   Can't just check on game join because the game
		//   returns 0 exp at the time of that event
		expWatcher.GetTotalGameExp();
		expWatcher.GetTotalZoneExp();
	}
}
