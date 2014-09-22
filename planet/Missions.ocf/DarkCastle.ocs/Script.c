/**
	Evil Castle
	Desc
	
	@authors Sven2
*/

static const S2DD_InitialRelaunchs = 0;

static g_is_initialized;

static g_shroom1, g_shroom2, g_shroom3, g_shroom4, g_ruin1, g_ruin2, g_ruin3, g_elev1, g_elev2, g_farmer, g_king;

func DoInit(int first_player)
{
	// Shrooms
	g_shroom1->AddPoisonEffect(0,0); // floor left
	g_shroom2->AddPoisonEffect(0,0); // ceiling left
	g_shroom3->AddPoisonEffect(-20,0); // floor right
	g_shroom4->AddPoisonEffect(10,-10); // ceiling right
	// Message when first player enters shroom area
	ScheduleCall(nil, Scenario.ShroomCaveCheck, 21, 0xffffff);
	// Scorching village
	g_ruin1->AddScorch(-20,-10, -45, 50, 1500);
	g_ruin2->AddScorch(-15,42, 90, 50, 1200);
	g_ruin3->AddScorch(-12,18, 130, 80, 1300);
	// Horax
	g_king.JumpSpeed = 200;
	// Update AI stuff
	var fx;
	for (var enemy in FindObjects(Find_ID(Clonk), Find_Owner(NO_OWNER)))
		if (fx = S2AI->GetAI(enemy))
		{
			fx.weapon = fx.target = nil;
			S2AI->BindInventory(enemy);
			enemy->DoEnergy(10000);
			enemy->AddEnergyBar();
		}
	// Intro. Message 250 frames + regular message time
	g_farmer.portrait = { Source=DialogueCastle };
	Dialogue->MessageBoxAll("$MsgIntro1$", g_farmer, true);
	Schedule(nil, "Dialogue->MessageBoxAll(\"$MsgIntro1$\", g_farmer, true)", 250, 1);
	return true;
}

func InitializePlayer(int plr)
{
	// Players only
	if (GetPlayerType(plr)!=C4PT_User) return;
	// Scenario init
	if (!g_is_initialized) g_is_initialized = DoInit(plr);
	// Harsh zoom range
	for (var flag in [PLRZOOM_LimitMax, PLRZOOM_Direct])
		SetPlayerZoomByViewRange(plr,400,250,flag);
	SetPlayerViewLock(plr, true);
	// Initial join
	var crew = GetCrew(plr);
	crew->SetPosition(35 + Random(10) , 1140);
	crew->SetDir(DIR_Right);
	crew->CreateContents(Shovel);
	crew->CreateContents(Hammer);
	crew->CreateContents(Axe);
	return true;
}


/* Enemy encounter messages */

func EncounterCave(object enemy, object player)
{
	Dialogue->MessageBoxAll("$MsgEncounterCave$", enemy, true);
	return true;
}

func EncounterOutpost(object enemy, object player)
{
	Dialogue->MessageBoxAll("$MsgEncounterOutpost$", enemy, true);
	return true;
}

func EncounterKing(object enemy, object player)
{
	if (!player) player = enemy; // Leads to a funny message, but better than a null pointer.
	Dialogue->MessageBoxAll(Format("$MsgEncounterKing$", player->GetName()), enemy, true);
	return true;
}


/* Mushroom cave encounter */

func ShroomCaveCheck()
{
	var intruder = FindObject(Find_InRect(1252,1342,320,138), Find_OCF(OCF_CrewMember));
	if (!intruder) return true;
	Dialogue->MessageBoxAll("$MsgEncounterShrooms$", intruder, true);
	ClearScheduleCall(nil, Scenario.ShroomCaveCheck);
	return true;
}

func OnGameOver()
{
	GainMissionAccess("S2Castle");
	return true;
}
