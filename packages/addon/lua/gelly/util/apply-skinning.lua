local function applySkinning(entity, bindPoses, vertex)
	-- since theres no w component, it's pretty much just a translation which is fine
	local finalVertex = Vector(0, 0, 0)
	local vertexPosition = vertex.pos

	-- we can iterate over the attached bones only to save time
	if not vertex.weights then
		return entity:GetWorldTransformMatrix() * vertexPosition
	end

	for _, weightData in ipairs(vertex.weights) do
		local boneID = weightData.bone
		local weight = weightData.weight
		local bindPoseMatrix = bindPoses[boneID].matrix
		local transformedVertex = entity:GetBoneMatrix(boneID)
			* bindPoseMatrix
			* vertexPosition
			* weight

		finalVertex = finalVertex + transformedVertex
	end

	return finalVertex
end

return applySkinning
