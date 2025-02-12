gellyx = gellyx or {}
gellyx.emitters = gellyx.emitters or {}

--- Aligns the given position to a grid centered at (0, 0, 0) with the given spacing.
---@param position Vector
---@param spacing number
---@return Vector position The aligned position.
local function alignToGrid(position, spacing)
	return Vector(
		math.floor(position.x / spacing) * spacing,
		math.floor(position.y / spacing) * spacing,
		math.floor(position.z / spacing) * spacing
	)
end

--- Parameters for a cube emitter, bounds is a local-space vector which determines the size of the cube.
---@alias gx.emitters.CubeParams {center: Vector, velocity: Vector, bounds: Vector, density: number, transform: VMatrix, material: table|nil, invMass: number|nil}

--- Emits particles in a cube shape.
---@param params gx.emitters.CubeParams Parameters for the emitter.
---@return boolean Whether the particles could be added.
function gellyx.emitters.Cube(params)
	if gellyx.isParticleLimitReached(params.density) then
		return false
	end

	---@type table<number, gx.ParticleSpawnData>
	local particles = {}
	local gridSpacing = gellyx.presets.getActivePreset().Radius / 4

	-- sample random particles within the cube, so we dont discretize the bounds into actual blocks
	for i = 1, params.density do
		local x = math.random() * 2 - 1
		local y = math.random() * 2 - 1
		local z = math.random() * 2 - 1

		local position = params.center + Vector(x, y, z) * params.bounds
		position = alignToGrid(position, gridSpacing)

		local velocity = params.velocity
		if params.transform then
			position = params.transform * position
			-- for velocity we don't want to transform the position of the velocity, just the direction
			local transformAngles = params.transform:GetAngles()
			local velocityDirection = velocity:GetNormalized()
			velocityDirection:Rotate(transformAngles)
			velocity = velocityDirection * velocity:Length()
		end

		particles[#particles + 1] = {
			pos = position,
			vel = velocity,
			invMass = params.invMass -- automatically defaults if its nil
		}
	end

	return gellyx.AddParticles(particles, params.material)
end
