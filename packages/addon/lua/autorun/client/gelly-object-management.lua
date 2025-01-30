local logging = include("gelly/logging.lua")
local array = include("gelly/util/functional-arrays.lua")

local objects = {}

local WHITELISTED_ENTITY_CLASSES = {
	"prop_physics",
	"prop_ragdoll",
	"player",
	"npc_*",
	"gmod_wheel",
	"func_*",
	"prop_door_rotating",
}

local function isClassWhitelisted(entity)
	if entity:GetClass() == "player" and gellyx.settings.get("player_collision"):GetInt() == 0 then
		return false
	end

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

local function getNormalizedModelName(entity)
	return entity:GetModel()[1] == "*" and entity:GetModel() or entity:GetModel():sub(1, -5)
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
	local normalizedModelName = getNormalizedModelName(entity)

	local success, msg = pcall(gelly.AddObject, normalizedModelName, entity:EntIndex())
	if not success then
		logging.warn("Failed to add object #%d to gelly", entity:EntIndex())
		logging.warn("Reason: %s", msg)
		return
	end

	table.insert(objectHandles, entity:EntIndex())
	entity.Gelly_ModelOnAdd = normalizedModelName
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

local function updateObjectBones(entity)
	local physicsBoneData = gelly.GetPhysicsBoneData(entity:GetModel())
	local isSingleCollider = table.Count(physicsBoneData) == 1
	entity:InvalidateBoneCache()

	for name, boneId in pairs(physicsBoneData) do
		local gmodBone = entity:LookupBone(name)
		if not gmodBone then
			gmodBone = boneId
		end

		local position
		local angles
		local testPosition = entity:GetBonePosition(gmodBone)
		if isSingleCollider then
			-- We need to apply the bind pose since ragdolls and the like already have their bones transformed
			local worldTransform = entity:GetWorldTransformMatrix()
			position = worldTransform:GetTranslation()
			angles = worldTransform:GetAngles()
		elseif testPosition == entity:GetPos() then
			-- Try matrix
			local matrix = entity:GetBoneMatrix(gmodBone)

			if matrix then
				position = matrix:GetTranslation()
				angles = matrix:GetAngles()
			else
				position = entity:GetPos()
				angles = entity:GetAngles()
			end
		else
			local bonePos, boneAng = entity:GetBonePosition(gmodBone)
			local matrix = Matrix()
			matrix:SetTranslation(bonePos)
			matrix:SetAngles(boneAng)

			position = matrix:GetTranslation()
			angles = matrix:GetAngles()
		end

		-- If we don't have the matrix, then the game has stopped processing the entity
		-- Usually happens once it goes out of view, so it's not an error
		if not position then
			return
		end

		gelly.SetObjectPosition(entity:EntIndex(), position, boneId)
		gelly.SetObjectRotation(entity:EntIndex(), angles, boneId)
	end
end

local function updateObject(entity)
	local objectHandles = objects[entity]
	if not objectHandles then
		return
	end

	if not IsValid(entity) then
		-- Somehow, it got pass the entity removal check
		removeObject(entity)
		return
	end

	if getNormalizedModelName(entity) ~= entity.Gelly_ModelOnAdd then
		removeObject(entity)
		addObject(entity)
		updateObject(entity)
		return
	end

	local isEntitySolid = bit.band(entity:GetSolidFlags(), FSOLID_NOT_SOLID) == 0
	if not isEntitySolid then
		removeObject(entity) -- An entity can't get into this state naturally, so it's very likely to have been removed but not cleaned up
		return
	end

	updateObjectBones(entity)
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

	gellyx.settings.registerOnChange("player_collision", function(_, _, new)
		if new == "0" then
			if objects[LocalPlayer()] then
				removeObject(LocalPlayer())
			end
		else
			if not objects[LocalPlayer()] then
				addObject(LocalPlayer())
			end
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
