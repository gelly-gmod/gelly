gellyx = gellyx or {}

local function maxParticles()
	return gelly.GetStatus().MaxParticles
end

local function activeParticles()
	return gelly.GetStatus().ActiveParticles
end

local PARTICLE_EMIT_DELAY = 0.01
local particleEmissionQueue = {}

local function pushParticleEmitRequest(particles)
	particleEmissionQueue[#particleEmissionQueue + 1] = particles
end

local function popParticleEmitRequest()
	if #particleEmissionQueue == 0 then
		return nil
	end

	return table.remove(particleEmissionQueue, 1)
end

local function isParticleRequestOverflowable(rawParticles)
	return activeParticles() + (#rawParticles / 2) > maxParticles()
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

	pushParticleEmitRequest(rawParticles)
	return true
end

timer.Create("gellyx.particle.queue", PARTICLE_EMIT_DELAY, 0, function()
	local particles = popParticleEmitRequest()
	if particles and not isParticleRequestOverflowable(particles) then
		gelly.AddParticles(particles, GELLY_ACTIVE_PRESET.Material.Absorption)
	end
end)
