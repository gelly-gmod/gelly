gellyx = gellyx or {}
gellyx.emitters = gellyx.emitters or {}

--- Parameters for a cube emitter, bounds is a local-space vector which determines the size of the cube.
---@alias gx.emitters.SphereParams {center: Vector, radius: number, velocity: Vector, density: number, randomness: number}

--- Emits particles in a cube shape.
---@param params gx.emitters.SphereParams Parameters for the emitter.
---@return boolean Whether the particles could be added.
function gellyx.emitters.Sphere(params)
	---@type table<number, gx.ParticleSpawnData>
	local particles = {}
	local velocityMagnitude = params.velocity:Length()
	params.randomness = params.randomness or 0

	for _ = 1, params.density do
		-- biased towards the poles- but its negligible, a better way would to use a gaussian distribution and normalize
		local theta = math.random() * 2 * math.pi
		local phi = math.random() * math.pi
		local r = math.random() * params.radius

		local position = params.center
			+ Vector(
				r * math.sin(phi) * math.cos(theta),
				r * math.sin(phi) * math.sin(theta),
				r * math.cos(phi)
			)

		local randomVelocity = VectorRand(-1, 1)
		table.insert(particles, {
			pos = position,
			vel = (
				params.velocity * (1 - params.randomness)
				+ randomVelocity * params.randomness
			) * velocityMagnitude,
		})
	end

	return gellyx.AddParticles(particles)
end
