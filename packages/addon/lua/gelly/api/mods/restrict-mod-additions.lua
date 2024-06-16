--- We don't want an addition by a mod, say a weapon, to be used if the mod is not loaded.

local array = include("gelly/util/functional-arrays.lua")
local logging = include("gelly/logging.lua")
local repository = include("gelly/api/mods/mod-repository.lua")

--- Restricts any entities/weapons loaded by any disabled mods at the time of calling.
return function()
	array(gellyx.mods.getLoadedMods())
		:filter(function(mod)
			return #mod.Entities > 0 or #mod.Weapons > 0
		end)
		:map(function(mod)
			return {
				enabled = repository.fetchMetadataForModId(mod.ID).enabled,
				weapons = array(mod.Weapons),
				entities = array(mod.Entities),
			}
		end)
		:forEach(function(restrictInfo)
			restrictInfo.weapons:forEach(function(weapon)
				logging.info("%s weapon %s", restrictInfo.enabled and "Enabling" or "Disabling", weapon)
				list.GetForEdit("Weapon")[weapon].Spawnable = restrictInfo.enabled
			end)

			restrictInfo.entities:forEach(function(entity)
				logging.info("%s entity %s", restrictInfo.enabled and "Enabling" or "Disabling", entity)
				list.GetForEdit("SpawnableEntities")[entity].Spawnable = restrictInfo.enabled
			end)

			RunConsoleCommand("spawnmenu_reload")
		end)
end
