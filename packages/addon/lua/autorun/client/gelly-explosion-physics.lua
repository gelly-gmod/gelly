hook.Add("GellyLoaded", "gelly.initialize-explosion-physics", function()
	hook.Add("GellyXExplosion", "gelly.on-explosion", function(pos, magnitude)
		local forcefield = gellyx.forcefield.create({
			Position = pos - Vector(0, 0, 20), -- slight offset to cause it to launch upwards
			Radius = 20 + magnitude * 2,
			Strength = magnitude * 600,
			LinearFalloff = true,
			Mode = gellyx.forcefield.Mode.Impulse,
		})

		timer.Simple(0.1, function()
			if IsValid(forcefield) then
				forcefield:Remove()
			end
		end)
	end)
end)
