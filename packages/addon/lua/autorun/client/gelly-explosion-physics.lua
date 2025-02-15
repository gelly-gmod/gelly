local Z_OFFSET = 20
local STRENGTH_MULTIPLIER = 600
local RADIUS_MULTIPLIER = 2
local MIN_RADIUS = 20

hook.Add("GellyLoaded", "gelly.initialize-explosion-physics", function()
	hook.Add("GellyXExplosion", "gelly.on-explosion", function(pos, magnitude)
		local forcefield = gellyx.forcefield.create({
			Position = pos - Vector(0, 0, Z_OFFSET), -- slight offset to cause it to launch upwards
			Radius = MIN_RADIUS + magnitude * RADIUS_MULTIPLIER,
			Strength = magnitude * STRENGTH_MULTIPLIER,
			LinearFalloff = true,
			Mode = gellyx.forcefield.Mode.Impulse,
		})

		timer.Simple(0.2, function()
			if IsValid(forcefield) then
				forcefield:Remove()
			end
		end)
	end)
end)
