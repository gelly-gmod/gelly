gellyx = gellyx or {}
gellyx.emitters = gellyx.emitters or {}

--- Parameters for a cube emitter, bounds is a local-space vector which determines the size of the cube.
---@alias gx.emitters.CubeParams {center: Vector, velocity: Vector, bounds: Vector, density: number}

--- Emits particles in a cube shape.
---@param params gx.emitters.CubeParams Parameters for the emitter.
---@return boolean Whether the particles could be added.
function gellyx.emitters.Cube(params)
	---@type table<number, gx.ParticleSpawnData>
	local particles = {}
	for _ = 1, params.density do
		local x = math.random() - 0.5
		local y = math.random() - 0.5
		local z = math.random() - 0.5

		local position = params.center + Vector(x, y, z) * params.bounds

		table.insert(particles, {
			pos = position,
			vel = params.velocity,
		})
	end

	return gellyx.AddParticles(particles)
end
