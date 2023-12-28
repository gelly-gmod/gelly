local function emitCube(center, velocity, size, density)
	local particles = {}
	for _ = 1, density do
		local x = math.random() - 0.5
		local y = math.random() - 0.5
		local z = math.random() - 0.5

		local position = center + Vector(x, y, z) * size
		local velocity = velocity

		table.insert(particles, position)
		table.insert(particles, velocity)
	end

	gelly.AddParticles(particles)
end

return emitCube
