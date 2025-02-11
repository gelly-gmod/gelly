gellyx = gellyx or {}
gellyx.mods = gellyx.mods or {}

local repository = include("gelly/api/mods/mod-repository.lua")
local findAllGellyMods = include("gelly/api/mods/find-all-gelly-mods.lua")
local array = include("gelly/util/functional-arrays.lua")
local logging = include("gelly/logging.lua")

local loadedMods = {}

local DEFAULT_MOD = "sandbox-mod"
gellyx.mods.DEFAULT_MOD = DEFAULT_MOD

include("gelly/api/mods/enums.lua")

function gellyx.mods.initialize()
	logging.info("Initializing mods.")
	loadedMods = findAllGellyMods()
	logging.info(("Found %d mods."):format(#loadedMods))

	array(loadedMods)
		:map(function(mod)
			return { isWithoutMetadata = repository.fetchMetadataForModId(mod.ID) == nil, info = mod }
		end)
		:filter(function(mod)
			return mod.isWithoutMetadata
		end)
		:forEach(function(mod)
			logging.info(("Mod %s is missing metadata, inserting default metadata."):format(mod.info.ID))
			repository.upsertMetadataForModId(mod.info.ID, { enabled = DEFAULT_MOD == mod.info.ID and true or false })
		end)

	local enabledMods = array(loadedMods)
		:map(function(mod)
			return { metadata = repository.fetchMetadataForModId(mod.ID) }
		end)
		:filter(function(mod)
			return mod.metadata.enabled
		end)
		:toArray()

	if #enabledMods == 0 then
		logging.warn("No mods enabled. Enabling default mod.")
		gellyx.mods.setModEnabled(DEFAULT_MOD, true)
	end
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
	logging.info("Mod %s is now %s.", modId, enabled and "enabled" or "disabled")
end

--- Returns if a mod is enabled.
---@param modId string
---@return boolean
function gellyx.mods.isModEnabled(modId)
	local metadata = repository.fetchMetadataForModId(modId)
	return metadata and metadata.enabled
end

--- Runs all enabled mods.
---@return nil
function gellyx.mods.runMods()
	hook.Run("GellyModsShutdown")

	array(loadedMods)
		:filter(function(mod)
			local enabled = repository.fetchMetadataForModId(mod.ID).enabled
			return enabled
		end)
		:forEach(function(mod)
			local success, err = pcall(function()
				include(mod.InitPath)
			end)

			if not success then
				logging.error(("Failed to run mod %s: %s"):format(mod.ID, err))
			end
		end)
end

function gellyx.mods.getLoadedMods()
	return table.Copy(loadedMods)
end
