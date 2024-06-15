-- A command-line interface for the mod manager.

local array = include("gelly/util/functional-arrays.lua")
local logging = include("gelly/logging.lua")
local repository = include("gelly/api/mods/mod-repository.lua")

local SUBCOMMANDS = {
	["enable"] = function(args)
		local modId = args[1]
		if not modId then
			logging.error("No mod ID provided.")
			return
		end

		gellyx.mods.setModEnabled(modId, true)
	end,
	["disable"] = function(args)
		local modId = args[1]
		if not modId then
			logging.error("No mod ID provided.")
			return
		end

		gellyx.mods.setModEnabled(modId, false)
	end,
	["select"] = function(args)
		local modId = args[1]
		if not modId then
			logging.error("No mod ID provided.")
			return
		end

		array(gellyx.mods.getLoadedMods())
			:filter(function(mod)
				return repository.fetchMetadataForModId(mod.ID).enabled
			end)
			:forEach(function(mod)
				gellyx.mods.setModEnabled(mod.ID, false)
			end)

		gellyx.mods.setModEnabled(modId, true)
		gellyx.mods.runMods()
	end,
	["list"] = function()
		local mods = array(gellyx.mods.getLoadedMods())
			:map(function(mod)
				return { info = mod, metadata = repository.fetchMetadataForModId(mod.ID) }
			end)
			:map(function(mod)
				return ("%s (%s) - %s"):format(mod.info.Name, mod.info.ID,
					mod.metadata.enabled and "enabled" or "disabled")
			end)
			:join("\n")

		print(mods)
	end,
	["refresh"] = function()
		gellyx.mods.runMods()
	end,
}

hook.Add("GellyLoaded", "gelly.setup-mod-command", function()
	concommand.Add("gelly_mod", function(_, _, args)
		local subcommand = args[1]
		if not subcommand then
			logging.error("No subcommand provided.")
			return
		end

		local handler = SUBCOMMANDS[subcommand]
		if not handler then
			logging.error("Unknown subcommand %s.", subcommand)
			return
		end

		handler(array(args):drop(1):toArray())
	end, function(cmd)
		local subcommandNames = {}
		for name, _ in pairs(SUBCOMMANDS) do
			table.insert(subcommandNames, array({ cmd, name }):join(" "))
		end

		return subcommandNames
	end, "A command-line interface for the mod manager.")
end)
