return function(brushModel)
	local model = brushModel:GetModel()
	local clientSideModel = ents.CreateClientProp(model)

	-- brush models don't have a typical model so we retrieve the physmesh instead
	local physicsObject = clientSideModel:GetPhysicsObject()
	if not physicsObject then
		clientSideModel:Remove()
		return nil
	end

	local mesh = physicsObject:GetMesh()
	local triangleSoup = {}

	for _, vertex in ipairs(mesh) do
		table.insert(triangleSoup, vertex.pos)
	end

	clientSideModel:Remove()
	return triangleSoup
end
