local MAX_FORCEFIELD_SAMPLES = 60
local FORCEFIELD_RADIUS = 10
local FORCEFIELD_STRENGTH = 11
local FORCEFIELD_LIFETIME = 0.07
local STEP_BACK_FRACTION = 0.1

hook.Add("GellyLoaded", "gelly.initialize-bullet-physics", function()
	hook.Add("GellyXBulletFired", "gelly.bullet-physics", function(startPos, endPos)
		local forcefields = {}
		local distance = startPos:Distance(endPos)
		local forcefieldCount = math.min(math.ceil(distance / (FORCEFIELD_RADIUS * 2)), MAX_FORCEFIELD_SAMPLES)

		for sample = 0, forcefieldCount do
			local t = sample / forcefieldCount
			-- step back propulses the fluid in the direction of the bullet
			local pos = LerpVector(math.max(t - STEP_BACK_FRACTION, 0), startPos, endPos)

			local forcefield = gellyx.forcefield.create({
				Position = pos,
				Radius = FORCEFIELD_RADIUS * (gellyx.presets.getEffectiveRadius() / 3),
				Strength = FORCEFIELD_STRENGTH * (gellyx.presets.getEffectiveRadius() / 3),
				LinearFalloff = false,
				Mode = gellyx.forcefield.Mode.Force,
			})

			if forcefield then
				table.insert(forcefields, forcefield)
			end
		end

		timer.Simple(FORCEFIELD_LIFETIME, function()
			for _, forcefield in ipairs(forcefields) do
				if IsValid(forcefield) then
					forcefield:Remove()
				end
			end
		end)
	end)
end)
