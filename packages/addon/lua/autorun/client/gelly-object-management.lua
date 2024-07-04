local logging = include("gelly/logging.lua")
local getBrushModelMesh = include("gelly/util/get-brush-model-mesh.lua")
local array = include("gelly/util/functional-arrays.lua")

local objects = {}

-- Used to offset the object handles so that they don't conflict with GMOD's entity handles
-- once a multi-mesh object is added.
local MULTI_OBJECT_OFFSET = 65536

--- Returns a list of the individual meshes and their vertices of the given model.
---@param modelPath string
local function getVerticesOfModel(modelPath)
	local meshes, bindPoses = util.GetModelMeshes(modelPath, 1, 1)
	local vertices = {}

	-- We want to transform the vertices to the root of the model so that there's no visual mismatch
	local rootTransform = Matrix()
	if bindPoses then
		rootTransform = bindPoses[0].matrix
	end

	for _, mesh in ipairs(meshes) do
		local vertsForMesh = {}
		for _, vertex in ipairs(mesh.triangles) do
			table.insert(vertsForMesh, rootTransform * vertex.pos)
		end

		table.insert(vertices, vertsForMesh)
	end

	return vertices
end

local function getObjectMesh(entity)
	local isBrushModel = entity:GetModel()[1] == "*"

	if isBrushModel then
		local mesh = getBrushModelMesh(entity)
		if not mesh then
			logging.warn("Failed to get mesh for entity #%d", entity:EntIndex())
			return nil
		end

		return { mesh }
	else
		return getVerticesOfModel(entity:GetModel())
	end
end

local WHITELISTED_ENTITY_CLASSES = {
	"prop_physics",
	"gmod_wheel",
	"func_*"
}

local function isClassWhitelisted(entity)
	return array(WHITELISTED_ENTITY_CLASSES):any(function(class)
		local classSubstring = class
		if classSubstring[#classSubstring] == "*" then
			classSubstring = classSubstring:sub(1, -2)
		end

		if entity:GetClass():find(classSubstring) then
			return true
		end

		return false
	end)
end

local function addObject(entity)
	if not IsValid(entity) or not isClassWhitelisted(entity) then
		return
	end

	logging.info("Adding object #%d to gelly", entity:EntIndex())
	local meshes = getObjectMesh(entity)

	if not meshes then
		logging.warn("Failed to get mesh for entity #%d, not adding to Gelly.", entity:EntIndex())
		return
	end

	local objectHandles = {}
	local offset = #meshes > 1 and MULTI_OBJECT_OFFSET or 0

	for _, mesh in ipairs(meshes) do
		table.insert(objectHandles, entity:EntIndex() + offset)
		gelly.AddObject(mesh, entity:EntIndex() + offset)
		offset = offset + 1
	end

	objects[entity] = objectHandles
end

local function removeObject(entity)
	local objectHandles = objects[entity]
	logging.info("Removing object #%d from gelly", entity:EntIndex())

	for _, objectHandle in ipairs(objectHandles) do
		gelly.RemoveObject(objectHandle)
	end

	objects[entity] = nil
end

local function updateObject(entity)
	local objectHandles = objects[entity]

	for _, objectHandle in ipairs(objectHandles) do
		if not IsValid(entity) then
			-- Somehow, it got pass the entity removal check
			removeObject(entity)
			return
		end

		if entity == LocalPlayer() then
			gelly.SetObjectPosition(objectHandle, entity:GetPos())
			gelly.SetObjectRotation(objectHandle, Angle(90, 0, 0))
			return
		end

		local transform = entity:GetBoneMatrix(0)
		if not transform then
			transform = entity:GetWorldTransformMatrix()
			if not transform then
				logging.warn("Transform bug for entity #%d", entity:EntIndex())
				removeObject(entity)
				return
			end
		end

		gelly.SetObjectPosition(objectHandle, transform:GetTranslation())
		gelly.SetObjectRotation(objectHandle, transform:GetAngles())
	end
end

local PLAYER_RADIUS = 15
local PLAYER_HALFHEIGHT = 16
hook.Add("GellyLoaded", "gelly.object-management-initialize", function()
	-- fetch any entities that may've been created before the hook was added
	timer.Simple(0.1,
		function() -- we arbitrarily wait 100ms to ensure the frame has ended (we can't really create entities during rendering)
			array(ents.GetAll()):forEach(addObject)
		end)

	hook.Add("OnEntityCreated", "gelly.object-add", function(entity)
		addObject(entity)
	end)

	hook.Add("EntityRemoved", "gelly.object-remove", function(entity)
		if not objects[entity] then
			return
		end

		removeObject(entity)
	end)

	hook.Add("Think", "gelly.object-update", function()
		for entity, _ in pairs(objects) do
			updateObject(entity)
		end
	end)
end)

hook.Add("GellyRestarted", "gelly.object-management-recreate-entities", function()
	local entities = {}
	for entity, _ in pairs(objects) do
		table.insert(entities, entity)
	end

	objects = {}
	for _, entity in ipairs(entities) do
		addObject(entity)
	end
end)
