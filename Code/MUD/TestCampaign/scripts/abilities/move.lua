if Parameters[1] == nil then
	WriteFailure("No direction was specified.")
else if "northwards":sub(1,#Parameters[1]) == Parameters[1] then
	if Room.Exits.North != nil then
		Caster.MoveTo( Room.Exits.North )
		Caster.Stats.Fatigue -= Room.Properties.FatigueDrain
	else
		WriteFailure("Your path is blocked.")
	end
else if "eastwards":sub(1,#Parameters[1]) == Parameters[1] then
	if Room.Exits.East != nil then
		Caster.MoveTo( Room.Exits.East )
		Caster.Stats.Fatigue -= Room.Properties.FatigueDrain
	else
		WriteFailure("Your path is blocked.")
	end
else if "southwards":sub(1,#Parameters[1]) == Parameters[1] then
	if Room.Exits.South != nil then
		Caster.MoveTo( Room.Exits.South )
		Caster.Stats.Fatigue -= Room.Properties.FatigueDrain
	else
		WriteFailure("Your path is blocked.")
	end
else if "westwards":sub(1,#Parameters[1]) == Parameters[1] then
	if Room.Exits.West != nil then
		Caster.MoveTo( Room.Exits.West )
		Caster.Stats.Fatigue -= Room.Properties.FatigueDrain
	else
		WriteFailure("Your path is blocked.")
	end
else if "upwards":sub(1,#Parameters[1]) == Parameters[1] then
	if Room.Exits.Up != nil then
		Caster.MoveTo( Room.Exits.Up )
		Caster.Stats.Fatigue -= Room.Properties.FatigueDrain
	else
		WriteFailure("Your path is blocked.")
	end
else if "downwards":sub(1,#Parameters[1]) == Parameters[1] then
	if Room.Exits.Down != nil then
		Caster.MoveTo( Room.Exits.Down )
		Caster.Stats.Fatigue -= Room.Properties.FatigueDrain
	else
		WriteFailure("Your path is blocked.")
	end
else
	WriteFailure("Unknown direction.")
end