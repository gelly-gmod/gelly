function EFFECT:Init(data)
	-- Because CEffectData is a shared object, we can't just store it and use its' properties later
	-- Instead, we store the properties themselves
	effects.BeamRingPoint(data:GetOrigin(), 1, 1, 400, 5, 4, Color(50, 50, 255))
end

function EFFECT:Think()
	return false
end

function EFFECT:Render()
end
