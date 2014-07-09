--Test Campaign--
Campaign = {
	Name = "Test Campaign",
	Author = "Andrew",
	Description = {
		Short = "A quick campaign made for testing the engine.",
		Long = 	"A small campaign created to provide a working" ..
				"example for future designers to look at and " ..
				"figure out how to create their own campaign."
	},
	Difficulty = "Easy",
	Length = "Short",
	Universe = {
		Abilities = {
			{
				Name = "Hide",
				Script = "scripts/abilities/hide.lua"
			},
			{
				Name = "Move",
				Script = "scripts/abilities/move.lua"
			}
		},
		Actors = {
			{
				Name = "Player Character",
				Script = "scripts/actors/playercharacter.lua"
			},
			{
				Name = "Inn Keeper",
				Script = "scripts/actors/innkeeper.lua"
			},
			{
				Name = "Ferocious Dog",
				Script = "scripts/actors/dog.lua"
			}
		},
		Behaviors = {
		
		},
		Dialogue = {
		
		},
		Effects = {
		
		},
		Factions = {
		
		},
		Objects = {
		
		},
		Quests = {
		
		},
		Shops = {
		
		},
		Spells = {
		
		},
		Realms = {
			{
				Name = "Divine",
				Script = "scripts/realms/divine.lua",
				Zones = {
					{
						Name = "Heaven",
						Script = "scripts/realms/divine/heaven.lua",
						Rooms = {
							{
								Name = "Heaven",
								Script = "scripts/realms/divine/heaven/heaven.lua"
							}
						}
					}
				}
			},
			{
				Name = "Earth",
				Script = "scripts/realms/earth.lua",
				Zones = {
					{
						Name = "Desert",
						Script = "scripts/realms/earth/desert.lua",
						Rooms = {
							{
								Name = "Windy Passage",
								Script = "scripts/realms/earth/desert/windypass.lua"
							}
						}
					},
					{
						Name = "Dungeon",
						Script = "scripts/realms/earth/dungeon.lua",
						Rooms = {
							{
								Name = "Dog's Cave",
								Script = "scripts/realms/earth/dungoen/dogcave.lua"
							},
							{
								Name = "Drippy Corridor",
								Script = "scripts/realms/earth/dungeon/drippycorridor.lua"
							}
						}
					},
					{
						Name = "Town",
						Script = "scripts/realms/earth/town.lua",
						Rooms = {
							{
								Name = "Inn",
								Script = "scripts/realms/earth/town/inn.lua"
							},
							{
								Name = "Town Square",
								Script = "scripts/realms/earth/town/townsquare.lua"
							}
						}
					},
					
				}
			},
			
		}
	}
}