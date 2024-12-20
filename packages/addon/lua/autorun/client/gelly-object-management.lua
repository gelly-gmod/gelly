local logging = include("gelly/logging.lua")
local array = include("gelly/util/functional-arrays.lua")

local objects = {}

local WHITELISTED_ENTITY_CLASSES = {
	"prop_physics",
	"gmod_wheel",
	"func_*",
	"prop_door_rotating",
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
	if not entity:GetModel() then
		logging.warn("Failed to get mesh for entity #%d, not adding to Gelly.", entity:EntIndex())
		return
	end

	local objectHandles = {}
	local normalizedModelName = entity:GetModel()[1] == "*" and entity:GetModel() or entity:GetModel():sub(1, -5)

	local success, msg = pcall(gelly.AddObject, normalizedModelName, entity:EntIndex())
	if not success then
		logging.warn("Failed to add object #%d to gelly", entity:EntIndex())
		logging.warn("Reason: %s", msg)
		return
	end

	table.insert(objectHandles, entity:EntIndex())
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
	if not objectHandles then
		return
	end

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

		local transform = entity:GetWorldTransformMatrix()
		if not transform then
			logging.warn("Transform bug for entity #%d", entity:EntIndex())
			removeObject(entity)
			return
		end

		gelly.SetObjectPosition(objectHandle, transform:GetTranslation())
		gelly.SetObjectRotation(objectHandle, transform:GetAngles())
	end
end

local function onPropResized(entity, newScale)
	local objectHandles = objects[entity]
	if not objectHandles then
		return
	end

	for _, objectHandle in ipairs(objectHandles) do
		gelly.SetObjectScale(objectHandle, newScale)
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
		timer.Simple(0, function()
			-- Empirical fix for teleportation:
			-- After much experimentation, Gelly was proven to be properly handling entity updates,
			-- but GMod is actually the issue--giving us the wrong entity position immediately after creation.
			addObject(entity)
			updateObject(entity)
		end)
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

	hook.Add("GellyXPropResized", "gelly.object-resize", onPropResized)
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
