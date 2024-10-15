---@module "gelly.util.apply-skinning"
local applySkinning = include("gelly/util/apply-skinning.lua")

gellyx = gellyx or {}
gellyx.emitters = gellyx.emitters or {}

--- Prevents huge amounts of particles from being emitted at once, particularly with ragdolls.
local MAX_MESH_PARTICLES = 100000

--- Parameters for the mesh emitter, density controls how many particles are emitted per triangle of the mesh.
---@alias gx.emitters.MeshParams {entity: Entity, density: number, material: table|nil}

--- Emits particles in the shape of the given entity, using their visual mesh.
---@param params gx.emitters.MeshParams Parameters for the emitter.
---@return boolean Whether the particles could be added.
function gellyx.emitters.Mesh(params)
	local entity = params.entity

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
			-- Weight the triangle by its area, our sampling algorithm will favor smaller triangles to compensate for their smaller area
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

	---@type table<number, gx.ParticleSpawnData>
	local particles = {}

	local function sampleEquidistantly(particles, triangle)
		local position = (triangle[1] + triangle[2] + triangle[3]) / 3

		table.insert(particles, {
			pos = position,
			vel = Vector(0, 0, 0),
		})
	end

	local function sampleRandomly(particles, triangle)
		local areaBoost = math.max(math.floor(triangle[4] * 50), 1)
		for _ = 1, params.density + areaBoost do
			local x = math.random()
			local y = math.random()
			if x + y > 1 then
				x = 1 - x
				y = 1 - y
			end

			local position = triangle[1] * x + triangle[2] * y + triangle[3] * (1 - x - y)

			table.insert(particles, {
				pos = position,
				vel = Vector(0, 0, 10),
			})
		end
	end

	local determineSampleMethod
	if isRagdoll then
		determineSampleMethod = function(area)
			return math.random() > area
		end
	else
		determineSampleMethod = function(area)
			return math.random() < 0.5
		end
	end

	for _, triangle in ipairs(triangles) do
		if determineSampleMethod(triangle[4]) then
			if #particles > MAX_MESH_PARTICLES then
				break
			end

			if isRagdoll then
				sampleEquidistantly(particles, triangle)
			else
				sampleRandomly(particles, triangle)
			end
		end
	end

	return gellyx.AddParticles(particles, params.material)
end
