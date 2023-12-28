---@module "gelly.util.apply-skinning"
local applySkinning = include("gelly/util/apply-skinning.lua")

local function emitMesh(entity, samplePerTriangle)
	local transform = entity:GetWorldTransformMatrix()
	local meshes, bindPoses = util.GetModelMeshes(entity:GetModel(), 1, 1)
	local isRagdoll = entity:GetClass() == "prop_ragdoll"

	local function transformVertex(vertex)
		if isRagdoll then
			return applySkinning(entity, bindPoses, vertex)
		else
			return transform * vertex.pos
		end
	end

	local triangles = {}
	for _, mesh in ipairs(meshes) do
		for i = 1, #mesh.triangles, 3 do
			local triangle = {}
			triangle[1] = transformVertex(mesh.triangles[i])
			triangle[2] = transformVertex(mesh.triangles[i + 1])
			triangle[3] = transformVertex(mesh.triangles[i + 2])
			-- Weight the triangle by its area, our sampling algorithm will favor larger triangles
			triangle[4] = (triangle[2] - triangle[1])
				:Cross(triangle[3] - triangle[1])
				:Length() / 2

			table.insert(triangles, triangle)
		end
	end

	-- Normalize the weights by the highest weight
	local maxWeight = 0
	for _, triangle in ipairs(triangles) do
		maxWeight = math.max(maxWeight, triangle[4])
	end

	for _, triangle in ipairs(triangles) do
		triangle[4] = triangle[4] / maxWeight
	end

	local particles = {}

	for _, triangle in ipairs(triangles) do
		if math.random() < triangle[4] then
			for _ = 1, samplePerTriangle do
				local x = math.random()
				local y = math.random()
				if x + y > 1 then
					x = 1 - x
					y = 1 - y
				end

				local position = triangle[1] * x
					+ triangle[2] * y
					+ triangle[3] * (1 - x - y)
				local velocity = Vector(0, 0, 0)
				table.insert(particles, position)
				table.insert(particles, velocity)
			end
		end
	end

	gelly.AddParticles(particles)
end

return emitMesh
