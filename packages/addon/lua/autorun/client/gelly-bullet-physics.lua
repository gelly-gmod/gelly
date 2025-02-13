local FORCEFIELD_SAMPLES = 24 -- # of equidistant forcefield points
local FORCEFIELD_RADIUS = 10
local FORCEFIELD_STRENGTH = 11
local FORCEFIELD_LIFETIME = 0.07
local STEP_BACK_FRACTION = 0.1

hook.Add("GellyLoaded", "gelly.initialize-bullet-physics", function()
	hook.Add("GellyXBulletFired", "gelly.bullet-physics", function(startPos, endPos)
		print("Yayaya")
		local forcefields = {}

		for sample = 0, FORCEFIELD_SAMPLES do
			local t = sample / FORCEFIELD_SAMPLES
			-- step back propulses the fluid in the direction of the bullet
			local pos = LerpVector(math.max(t - STEP_BACK_FRACTION, 0), startPos, endPos)

			local forcefield = gellyx.forcefield.create({
				Position = pos,
				Radius = FORCEFIELD_RADIUS,
				Strength = FORCEFIELD_STRENGTH,
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
