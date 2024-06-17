gellyx = gellyx or {}
gellyx.mods = gellyx.mods or {}

local repository = include("gelly/api/mods/mod-repository.lua")
local findAllGellyMods = include("gelly/api/mods/find-all-gelly-mods.lua")
local array = include("gelly/util/functional-arrays.lua")
local logging = include("gelly/logging.lua")
local restrictModAdditions = include("gelly/api/mods/restrict-mod-additions.lua")

local loadedMods = {}

local DEFAULT_MOD = "sandbox-mod"

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
	logging.info("Updating mod restrictions.")

	restrictModAdditions()
end

local function getGlobalModConflicts()
	-- two global mods can't be enabled at the same time
	local globalMods = array(loadedMods)
		:filter(function(mod)
			return mod.Type == gellyx.mods.ModType.Global
		end)
		:map(function(mod)
			return { info = mod, metadata = repository.fetchMetadataForModId(mod.ID) }
		end)
		:filter(function(mod)
			return mod.metadata.enabled
		end)
		:map(function(mod)
			return mod.info.ID
		end)
		:toArray()

	if #globalMods > 1 then
		return globalMods
	end

	return nil
end

--- Runs all enabled mods.
---@return nil
function gellyx.mods.runMods()
	hook.Run("GellyModsShutdown")

	local globalModConflicts = getGlobalModConflicts()

	if globalModConflicts then
		logging.error(("Global mods %s are conflicting."):format(table.concat(globalModConflicts, ", ")))
		return
	end

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

	restrictModAdditions()
end

function gellyx.mods.getLoadedMods()
	return table.Copy(loadedMods)
end
