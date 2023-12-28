local function emitMesh(entity, samplePerTriangle)
	local transform = entity:GetWorldTransformMatrix()
	local meshes = util.GetModelMeshes(entity:GetModel(), 1, 1)

	local triangles = {}
	for _, mesh in ipairs(meshes) do
		for i = 1, #mesh.triangles, 3 do
			local triangle = {}
			triangle[1] = transform * mesh.triangles[i].pos
			triangle[2] = transform * mesh.triangles[i + 1].pos
			triangle[3] = transform * mesh.triangles[i + 2].pos

			table.insert(triangles, triangle)
		end
	end

	local particles = {}

	for _, triangle in ipairs(triangles) do
		for _ = 1, samplePerTriangle do
			local x = math.random()
			local y = math.random()
			if x + y > 1 then
				x = 1 - x
				y = 1 - y
			end

			local position = triangle[1] * x + triangle[2] * y + triangle[3] * (1 - x - y)
			local velocity = Vector(0, 0, 0)
			print(position)
			table.insert(particles, position)
			table.insert(particles, velocity)
		end
	end

	gelly.AddParticles(particles)
end

return emitMesh
