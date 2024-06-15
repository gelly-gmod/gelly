local MODS_TABLE_NAME = "gelly_mods"

---@alias gellyx.mods.ModMetadata {enabled: boolean}

---@class gellyx.mods.ModRepository
--- Encapsulates the database operations for mods.
local repository = {}

function repository.initialize()
	if not sql.TableExists(MODS_TABLE_NAME) then
		sql.Query("CREATE TABLE " .. MODS_TABLE_NAME .. " (mod_id TEXT PRIMARY KEY, enabled BOOLEAN)")
	end
end

--- Upserts metadata for a mod ID.
---@param modId string
---@param metadata gellyx.mods.ModMetadata
---@return nil
function repository.upsertMetadataForModId(modId, metadata)
	local query = ("INSERT OR REPLACE INTO %s (mod_id, enabled) VALUES (%s, %s)"):format(
		MODS_TABLE_NAME,
		sql.SQLStr(modId),
		sql.SQLStr(metadata.enabled)
	)

	sql.Query(query)
end

--- Fetches metadata for a mod ID.
---@param modId string
---@return gellyx.mods.ModMetadata|nil metadata
function repository.fetchMetadataForModId(modId)
	local query = ("SELECT * FROM %s WHERE mod_id = %s"):format(MODS_TABLE_NAME, sql.SQLStr(modId))
	local result = sql.QueryValue(query)

	if not result then
		return nil
	end

	return { enabled = tobool(result.enabled) }
end

repository.initialize()
return repository
