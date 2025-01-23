-- One of the benefits of the mod system is that it'll never be ran on the server, so most conditional realm blocks are unnecessary.
print("Blood mod loaded")

local DAMAGE_TYPE_BLOOD_CONFIGS = {
	{
		DamageFlags = bit.bor(DMG_BULLET, DMG_ALWAYSGIB),
		MinDensity = 200,
		MaxDensity = 400,
		VelocityPower = -7, -- bullets usually are rotating so they can end up flinging blood
		Randomness = 0.7, -- spray in the direction of the normal
		CubeSize = 9,
		DamageMultiplier = 10, -- density is added by the damage * this
	},
	{
		DamageFlags = DMG_BLAST,
		MinDensity = 100,
		MaxDensity = 800,
		VelocityPower = 10,
		Randomness = 0.7,
		CubeSize = 2,
		DamageMultiplier = 200,
		FromEntity = true,
	},
	{
		DamageFlags = bit.bor(DMG_SLASH, DMG_CLUB),
		MinDensity = 200,
		MaxDensity = 2200,
		VelocityPower = -15,
		Randomness = 0.95,
		CubeSize = 10,
		DamageMultiplier = 5,
	},
	{
		DamageFlags = DMG_CRUSH,
		MinDensity = 500,
		MaxDensity = 700,
		VelocityPower = 2,
		Randomness = 2,
		CubeSize = 10,
		DamageMultiplier = 25,
	},
}

local WEAPON_BLOOD_CONFIGS = {
	-- double barrel shotgun in m9k
	m9k_dbarrel = {
		MinDensity = 100,
		MaxDensity = 200,
		VelocityPower = -14, -- bullets usually are rotating so they can end up flinging blood
		Randomness = 0.9, -- spray in the direction of the normal
		CubeSize = 52,
		DamageMultiplier = 35, -- density is added by the damage * this
	},

	-- shotgun
	weapon_shotgun = {
		MinDensity = 300,
		MaxDensity = 500,
		VelocityPower = -9, -- blood should be launched at the player from the wound not from the wound at the victim
		Randomness = 0.9,
		CubeSize = 7,
		DamageMultiplier = 50,
	},

	weapon_357 = {
		MinDensity = 100,
		MaxDensity = 200,
		VelocityPower = 5, -- a .357 bullet should maybe penetrate through the entire victim
		Randomness = 0.1,
		CubeSize = 10,
		DamageMultiplier = 20,
	},
}

local BLOOD_COLOR_ABSORPTION = {
	[BLOOD_COLOR_RED] = Vector(0.02, 0.1, 0.1) * 320,
	[BLOOD_COLOR_YELLOW] = Vector(0.02, 0.02, 0.1) * 320, -- yellow is formed by mixing red and green
	[BLOOD_COLOR_GREEN] = Vector(0.1, 0.02, 0.1) * 320,
	[BLOOD_COLOR_MECH] = Vector(10.0, 10.0, 10.0) * 320,
	[BLOOD_COLOR_ANTLION] = Vector(0.02, 0.02, 0.1) * 320, -- a little yellow-green
	[BLOOD_COLOR_ZOMBIE] = Vector(0.02, 0.05, 0.1) * 320,
	[BLOOD_COLOR_ANTLION_WORKER] = Vector(0.1, 0.02, 0.1) * 320,
}

local function getDamageTypeConfig(damageType)
	for _, config in ipairs(DAMAGE_TYPE_BLOOD_CONFIGS) do
		if bit.band(damageType, config.DamageFlags) ~= 0 then
			return config
		end
	end
	return nil
end

local function getConfig(attacker, damageType)
	local config = nil

	if attacker:IsValid() and attacker.GetActiveWeapon and IsValid(attacker:GetActiveWeapon()) then
		config = WEAPON_BLOOD_CONFIGS[attacker:GetActiveWeapon():GetClass()]
	end

	if damageType and not config then
		config = getDamageTypeConfig(damageType)
	end

	return config
end

local function sprayBlood(damageType, victim, attacker, position, force, damage, material)
	local config = getConfig(attacker, damageType)
	if not config then return end

	local entPos = (victim:IsNPC() or victim:IsPlayer()) and
		victim:GetBonePosition(0) or
		victim:GetPos()

	local normal = config.FromEntity and
		(entPos - position):GetNormalized() or
		attacker == LocalPlayer() and
		attacker:GetAimVector() or
		force:GetNormalized()

	local density = math.random(config.MinDensity, config.MaxDensity)
	local velocity = normal * config.VelocityPower

	local damageMultiplier = config.DamageMultiplier or 0
	density = density + damageMultiplier * damage

	gellyx.emitters.Sphere({
		center = config.FromEntity and
			entPos or
			position,
		velocity = velocity,
		radius = config.CubeSize,
		density = density,
		randomness = config.Randomness,
		material = material,
	})
end

hook.Add(
	"GellyXDamage",
	"gelly.builtin.blood-mod",
	function(victim, attacker, position, force, damage, type)
		if not victim:IsValid() then return end

		local bloodColor = victim:GetBloodColor() or BLOOD_COLOR_RED

		if bloodColor < 1 then bloodColor = BLOOD_COLOR_RED end

		sprayBlood(type, victim, attacker, position, force, damage, {
			Roughness = 0,         -- blood isn't rough at all
			IsSpecularTransmission = false, -- blood is translucent
			RefractiveIndex = 1.373, -- blood has a slightly higher refractive index than water
			Absorption = BLOOD_COLOR_ABSORPTION[bloodColor],
			DiffuseColor = Vector(0, 0, 0),
		})
	end
)

gellyx.presets.select("Blood")

hook.Add("GellyModsShutdown", "gelly.builtin.blood-mod", function()
	hook.Remove("GellyXDamage", "gelly.builtin.blood-mod")
	hook.Remove("OnEntityCreated", "gelly.blood-quantity")
	hook.Remove("CreateEntityRagdoll", "gelly.blood-quantity-ragdoll")
	-- remove any left over blood
	gelly.Reset()

	print("Blood mod unloaded")
end)
