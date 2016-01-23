/**
	Aerobatics
	Several small sky islands form a chaotic parkour with lots of usable and respawning items.
	
	TODO:
	horizontal mode with large map
	reset player cp completion to killer (idea)
	
	@author Maikel
*/


static checkpoint_locations;
static inventorslab_location;

protected func Initialize()
{
	// Create the parkour goal.
	var goal = CreateObject(Goal_Parkour);
	goal->TransferContentsOnRelaunch(true);
	
	// Iterate over checkpoint list and create the checkpoints.
	// Also add items spawns close to the checkpoints.
	var map_zoom = GetScenarioVal("MapZoom", "Landscape");
	for (var index = 0; index < GetLength(checkpoint_locations); index++)
	{
		var cp = checkpoint_locations[index];
		var x = map_zoom * cp[0] + map_zoom / 2;
		var y = map_zoom * cp[1] + map_zoom / 2 - 20;
		if (index == 0)
		{
			goal->SetStartpoint(x, y);
			ItemSpawn->Create(Dynamite, x - 30, y);
			ItemSpawn->Create(Loam, x + 30, y);
			continue;
		}
		if (index == GetLength(checkpoint_locations) - 1)
		{
			goal->SetFinishpoint(x, y);
			ItemSpawn->Create(Balloon, x + 30, y);			
			continue;
		}		
		var mode = PARKOUR_CP_Check | PARKOUR_CP_Ordered | PARKOUR_CP_Respawn | PARKOUR_CP_Bonus;
		goal->AddCheckpoint(x - 10, y, mode);
		var spawn_items = [Dynamite, Loam, DynamiteBox, IronBomb, Javelin];
		ItemSpawn->Create(spawn_items[Random(Min(index, GetLength(spawn_items)))], x + 20, y);
	}
	
	// Rules.
	CreateObject(Rule_NoPowerNeed);
	
	// Initialize parts of the scenario.
	var amount = BoundBy(SCENPAR_NrCheckPoints, 6, 20);
	InitMaterials(amount);
	InitEnvironment(amount);
	InitVegetation(amount);
	InitAnimals(amount);
	
	// Start the intro sequence.
	StartSequence("Intro", 0);
	return;
}

private func InitMaterials(int amount)
{
	PlaceObjects(Dynamite, 4 * amount, "Earth");
	PlaceObjects(Loam, 4 * amount, "Earth");
	PlaceObjects(Metal, 2 * amount, "Earth");
	
	// Place chests on several of the sky islands.
	for (var count = 0; count < amount / 2; count++)
	{
		var pos = FindIslandLocation();
		if (!pos)
			continue;	
		var chest = CreateObjectAbove(Chest, pos.x, pos.y);
		chest->CreateContents(Dynamite, 4);
		chest->CreateContents(Club);
		chest->CreateContents(Musket)->CreateContents(LeadShot);
		chest->CreateContents(Bow)->CreateContents(Arrow);
		chest->CreateContents(Bread, 2);
		chest->CreateContents(IronBomb);
		chest->CreateContents(Cloth);
		chest->CreateContents(Pickaxe);
		if (!Random(2))
			chest->CreateContents(GrenadeLauncher)->CreateContents(IronBomb);
		if (!Random(2))
			chest->CreateContents(Boompack);
		if (!Random(2))
			chest->CreateContents(WallKit);
		if (!Random(2))
			chest->CreateContents(TeleGlove);
	}
	// Place some catapults.
	for (var count = 0; count < amount / 4; count++)
	{
		var pos = FindIslandLocation();
		if (pos)
			CreateObjectAbove(Catapult, pos.x, pos.y);
	}	
	// Place some cannons.
	for (var count = 0; count < amount / 4; count++)
	{
		var pos = FindIslandLocation();
		if (pos)
		{
			var cannon = CreateObjectAbove(Cannon, pos.x, pos.y);
			cannon->CreateContents(PowderKeg, 1);
		}
	}
	// Place some ropeladders.
	for (var count = 0; count < 2; count++)
	{
		var pos = FindLocation(Loc_Sky(), Loc_Wall(CNAT_Top));	
		if (!pos)
			continue;
		CreateObject(Ropeladder, pos.x, pos.y)->Unroll(-1, COMD_Up);
	}
	// An inventor's lab on its island.
	var map_zoom = GetScenarioVal("MapZoom", "Landscape");
	var lab = CreateObjectAbove(InventorsLab, inventorslab_location[0] * map_zoom, inventorslab_location[1] * map_zoom);
	lab->MakeInvincible();
	return;
}

private func FindIslandLocation()
{
	var pos;
	for (var tries = 0; tries < 100; tries++)
	{
		pos = FindLocation(Loc_Sky(), Loc_Wall(CNAT_Bottom));
		if (!pos)
			continue;
		if (FindObject(Find_Category(C4D_Vehicle), Find_Distance(30, pos.x, pos.y)))
			continue;
		break;
	}
	return pos;
}

private func InitEnvironment(int amount)
{
	for (var count = 0; count < amount / 2; count++)
	{
		var pos = FindLocation(Loc_Sky(), Loc_Space(40), Loc_InRect(100, 100, LandscapeWidth() - 200, LandscapeHeight() - 200));
		if (!pos)
			return;
		AddEffect("WindStream", nil, 100, 1, nil, nil, RandomX(-10, 10), pos.x, pos.y);
	}
	return;
}

private func InitVegetation(int amount)
{
	Grass->Place(100);
	Branch->Place(3 * amount);
	Mushroom->Place(amount);
	Fern->Place(amount);
	Flower->Place(3 * amount / 2);
	Tree_Deciduous->Place(amount);
	Tree_Coniferous2->Place(amount / 4);
	Tree_Coniferous3->Place(amount / 4);
	Cotton->Place(amount / 2);
	return;
}

private func InitAnimals(int amount)
{
	Butterfly->Place(amount);
	Mosquito->Place(amount / 2);
	Zaphive->Place(amount / 4);
	return;
}


/*-- Player Control --*/

protected func InitializePlayer(int plr)
{
	// Make the player enemy to all other players.
	Goal_Melee->MakeHostileToAll(plr, GetPlayerTeam(plr));
	// Set large zoom ranges for the player.
	SetPlayerZoomByViewRange(plr, 1200, nil, PLRZOOM_LimitMax);
	// Give the player knowledge for items in the inventor's lab.
	SetPlrKnowledge(plr, WindBag);
	SetPlrKnowledge(plr, WallKit);
	return;
}

public func OnPlayerRespawn(int plr, object cp)
{
	var crew = GetCrew(plr);
	if (!crew)
		return;
	// Ensure at least a shovel, loam and dynamite on respawn when items not present and there is space in inventory.
	if (!FindObject(Find_ID(Shovel), Find_Container(crew)) && crew->ContentsCount() < crew.MaxContentsCount)
		crew->CreateContents(Shovel);
	if (!FindObject(Find_ID(Loam), Find_Container(crew)) && crew->ContentsCount() < crew.MaxContentsCount)
		crew->CreateContents(Loam);
	if (!FindObject(Find_ID(Dynamite), Find_Container(crew)) && crew->ContentsCount() < crew.MaxContentsCount)
		crew->CreateContents(Dynamite);
	// Give the crew more HP, so that it can survive more flint jumps.
	crew.MaxEnergy = 80000;
	crew->DoEnergy(crew.MaxEnergy / 1000);	
	return;
}

// Give the player a bonus when he reaches a new checkpoint for the first time and is behind the leader.
public func GivePlrBonus(int plr, object cp)
{
	var crew = GetCrew(plr);
	if (!crew)
		return;
	var goal = cp->GetCPController();
	if (!goal)
		return;
	var cp_behind = goal->GetLeaderClearedCheckpoints() - goal->GetPlayerClearedCheckpoints(plr);
	if (cp_behind <= 2)
		return;
	var windbag = FindObject(Find_ID(WindBag), Find_Container(crew));
	if (windbag)
	{
		windbag->SetUsageCount(windbag->GetUsageCount() + cp_behind);	
	}
	else
	{
		if (crew->ContentsCount() < crew.MaxContentsCount)
			windbag = crew->CreateContents(WindBag);
		else
			windbag = CreateObjectAbove(WindBag, cp->GetX(), cp->GetY() + cp->GetBottom(), plr);
		windbag->SetUsageCount(2 * cp_behind);
	}
	return;
}


/*-- Wind Streams --*/

global func FxWindStreamStart(object target, effect, int temp, angle, x, y)
{
	if (temp)
		return FX_OK;
	effect.xdir = Sin(angle, 32);
	effect.ydir = -Cos(angle, 32);
	effect.x = x + Sin(angle, 43);
	effect.y = y - Cos(angle, 43);
	effect.particles =
	{
		Prototype = Particles_Air(),
		Size = PV_Random(2, 5)
	};
	return FX_OK;
}

global func FxWindStreamTimer(object target, proplist effect, int time)
{
	var xdir = effect.xdir;
	var ydir = effect.ydir;
	var x = effect.x;
	var y = effect.y;
	
	if(time<36)
	{
		var r=Random(360);
		var d=Random(40);
		CreateParticle("Air", Sin(r,d)+x,-Cos(r,d)+y, xdir/3, ydir/3, PV_Random(10, 30), effect.particles, 1);
		return FX_OK;
	}
	CreateParticle("Air", PV_Random(x - 20, x + 20), PV_Random(y - 20, y + 20), xdir/2, ydir/2, PV_Random(10, 30), effect.particles, 5);
	
	for(var obj in FindObjects(Find_Distance(40,x,y), Find_Not(Find_Category(C4D_Structure)), Find_Layer()))
	{
		if(PathFree(x,y,obj->GetX(),obj->GetY()))
		{
			if(xdir<0)
			{if(obj->GetXDir() > xdir) obj->SetXDir(obj->GetXDir(100) + (xdir*3)/2,100); }
			else 
			{if(obj->GetXDir() < xdir) obj->SetXDir(obj->GetXDir(100) + (xdir*3)/2,100); }
			
			if(ydir<0)
			{if(obj->GetYDir() > ydir) obj->SetYDir(obj->GetYDir(100) + (ydir*3)/2,100); }
			else 
			{if(obj->GetYDir() < ydir) obj->SetYDir(obj->GetYDir(100) + (ydir*3)/2,100); }
		}
	}
	return FX_OK;
}

