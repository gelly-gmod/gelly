gellyx = gellyx or {}
gellyx.mods = gellyx.mods or {}

local repository = include("gelly/api/mods/mod-repository.lua")
local findAllGellyMods = include("gelly/api/mods/find-all-gelly-mods.lua")
local array = include("gelly/util/functional-arrays.lua")
local logging = include("gelly/logging.lua")

local loadedMods = {}

include("gelly/api/mods/enums.lua")

function gellyx.mods.initialize()
	loadedMods = findAllGellyMods()

	array(loadedMods)
		:map(function(mod)
			return { isWithoutMetadata = repository.fetchMetadataForModId(mod.ID) == nil, mod = mod }
		end)
		:filter(function(mod)
			return mod.isWithoutMetadata
		end)
		:forEach(function(mod)
			logging.info(("Mod %s is missing metadata, inserting default metadata."):format(mod.mod.ID))
			repository.upsertMetadataForModId(mod.mod.ID, { enabled = false })
		end)
end

--- Enables/disables a mod by its ID.
---@param modId string
---@param enabled boolean
---@return nil
function gellyx.mods.setModEnabled(modId, enabled)
	local currentMetadata = repository.fetchMetadataForModId(modId)

	if not currentMetadata then
		logging.error(("Mod %s does not exist."):format(modId))
		return
	end

	repository.upsertMetadataForModId(modId, { enabled = enabled })
end

local function getGlobalModConflicts()
	-- two global mods can't be enabled at the same time
	local globalMods = array(loadedMods)
		:filter(function(mod)
			return mod.Type == gellyx.mods.ModType.Global
		end)
		:map(function(mod)
			return { mod = mod, metadata = repository.fetchMetadataForModId(mod.ID) }
		end)
		:filter(function(mod)
			return mod.metadata.enabled
		end)
		:map(function(mod)
			return mod.mod.ID
		end)
		:toArray()

	if #globalMods > 1 then
		return globalMods
	end

	return nil
end

function gellyx.mods.runMods()

end
