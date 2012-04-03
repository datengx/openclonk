/**
	Elevator control
	This is to include in every vehicle that should fit into the elevator case. It will bypass the controls [Up] and [Down] to the elevator case.
	Also, only these objects will get snatched into the case.

	@author Clonkonaut
*/

// This object fits into an elevator case
public func FitsInElevator()
{
	return true;
}

/* Controls */

func ControlUp(object clonk)
{
	if (GetEffect("ElevatorControl", this)) return Control2Elevator(CON_Up, clonk);
	return _inherited(clonk);
}
func ControlDown(object clonk)
{
	if (GetEffect("ElevatorControl", this)) return Control2Elevator(CON_Down, clonk);
	return _inherited(clonk);
}
func ControlStop(object clonk, int control)
{
	if (GetEffect("ElevatorControl", this))
		if (control == CON_Up || control == CON_Down)
		{
			var effect = GetEffect("ElevatorControl", this);
			effect.controlled = nil;
			return effect.case->ControlStop(clonk, control);
		}
	return _inherited(clonk, control);
}

func Control2Elevator(int control, object clonk)
{
	var effect = GetEffect("ElevatorControl", this);
	if (control == CON_Up)
		effect.case->ControlUp(clonk);
	if (control == CON_Down)
		effect.case->ControlDown(clonk);
	effect.controlled = control;
}

/* Effect */

public func FxElevatorControlStart(object vehicle, proplist effect, int temp, object case)
{
	if (temp) return;
	effect.case = case;
}

public func FxElevatorControlTimer(object vehicle, proplist effect)
{
	if (ObjectDistance(effect.case, vehicle) > 12)
		return -1;
	if (effect.controlled && !FindObject(Find_Action("Push"), Find_ActionTarget(vehicle)))
	{
		effect.case->ControlStop(vehicle, effect.controlled);
		effect.controlled = nil;
	}
}

public func FxElevatorControlStop(object vehicle, proplist effect, int reason, bool temp)
{
	if (temp) return;
	if (effect.controlled)
		effect.case->ControlStop(FindObject(Find_Action("Push"), Find_ActionTarget(vehicle)), effect.controlled);
}