-- Since gelly-gmod is a clientside thing, we just need to add the gelly files to the client's download list.

---@module "gelly.logging"
local logging = include("gelly/logging.lua")

local function traverseFileTree(startingDirectory, visitFunction)
	local files, directories = file.Find(startingDirectory .. "/*", "GAME")

	for _, fileName in ipairs(files) do
		visitFunction(startingDirectory .. "/" .. fileName)
	end

	for _, directory in ipairs(directories) do
		traverseFileTree(startingDirectory .. "/" .. directory, visitFunction)
	end
end

traverseFileTree("lua/gelly", function(path)
	logging.info("Adding file %s to download list", path)
	AddCSLuaFile(path)
end)

resource.AddSingleFile("particles/gelly.pcf")
game.AddParticles("particles/gelly.pcf")
PrecacheParticleSystem("Liquifier_ChargePrepare")
PrecacheParticleSystem("Liquifier_ChargeBlast")
