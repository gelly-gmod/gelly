gellyx = gellyx or {}

local function maxParticles()
	return gelly.GetStatus().MaxParticles
end

local function activeParticles()
	return gelly.GetStatus().ActiveParticles
end

--- Structure representing an individual particle to be spawned.
---@alias gx.ParticleSpawnData {pos: Vector, vel: Vector}

--- Adds the given particle data to the simulation, returning true if they could be added, false otherwise.
---@param particles table<number, gx.ParticleSpawnData>
---@return boolean
function gellyx.AddParticles(particles)
	local rawParticles = {}
	if activeParticles() + #particles > maxParticles() then
		return false
	end

	for _, spawnData in ipairs(particles) do
		rawParticles[#rawParticles + 1] = spawnData.pos
		rawParticles[#rawParticles + 1] = spawnData.vel
	end

	gelly.AddParticles(rawParticles)
	return true
end
