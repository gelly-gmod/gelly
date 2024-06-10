-- One of the benefits of the mod system is that it'll never be ran on the server, so most conditional realm blocks are unnecessary.
print("Blood mod loaded")

local BLOOD_CONFIGS = {
    {
        DamageFlags = bit.bor(DMG_BULLET, 8194),
        MinDensity = 200,
		MaxDensity = 400,
		VelocityPower = 10, -- bullets usually are rotating so they can end up flinging blood
		Randomness = 0.8, -- spray in the direction of the normal
		CubeSize = 9,
		DamageMultiplier = 15, -- density is added by the damage * this
    },
    {
        damageFlags = DMG_BLAST,
        MinDensity = 3200,
		MaxDensity = 5200,
		VelocityPower = 40,
		Randomness = 1,
		CubeSize = 10,
		DamageMultiplier = 90,
    },
	{
        damageFlags = bit.bor(DMG_SLASH, DMG_CLUB),
        MinDensity = 100,
		MaxDensity = 200,
		VelocityPower = 4,
		Randomness = 0.1,
		CubeSize = 3,
    },
	{
        damageFlags = DMG_CRUSH,
        MinDensity = 100,
		MaxDensity = 200,
		VelocityPower = 4,
		Randomness = 1,
		CubeSize = 15,
		DamageMultiplier = 20,
    },
}

--[[ local OLDBLOOD_CONFIGS = { -- IMPLEMENT SPECIFIC WEAPON DAMAGE
	-- double barrel shotgun in m9k
	[4098] = {
		MinDensity = 100,
		MaxDensity = 200,
		VelocityPower = 14, -- bullets usually are rotating so they can end up flinging blood
		Randomness = 0.9, -- spray in the direction of the normal
		CubeSize = 52,
		DamageMultiplier = 35, -- density is added by the damage * this
	},

	-- shotgun
	[536870914] = {
		MinDensity = 100,
		MaxDensity = 200,
		VelocityPower = 14,
		Randomness = 0.1,
		CubeSize = 5,
	},
} ]]

local function getConfig(damageType)
	for _, config in ipairs(BLOOD_CONFIGS) do
        if bit.band(damageType, config.damageFlags) ~= 0 then
            return config
        end
	end
	return nil
end

local function sprayBlood(damageType, attacker, position, force, damage)
	local normal = Vector()
	if attacker == LocalPlayer() then
		normal = LocalPlayer():GetAimVector()
	else
		normal = force:GetNormalized()
	end

	local config = getConfig(damageType)
	if not config then return end

	local density = math.random(config.MinDensity, config.MaxDensity)
	local velocity = normal * config.VelocityPower

	local bounds = Vector(config.CubeSize, config.CubeSize, config.CubeSize)
	local damageMultiplier = config.DamageMultiplier or 0
	density = density + damageMultiplier * damage

	gellyx.emitters.Sphere({
		center = position,
		velocity = velocity,
		radius = config.CubeSize,
		density = density,
		randomness = config.Randomness,
	})
end

hook.Add(
	"GellyXDamage",
	"gelly.builtin.blood-mod",
	function(victim, attacker, position, force, damage, type)
		if
			not victim:IsValid() or
			not victim:IsPlayer() and
			not victim:IsNPC() and
			victim:GetClass() ~= "prop_ragdoll"
		then
			return
		end

		local bloodcolor = 0
		
		if victim:GetBloodColor() ~= nil then
			bloodcolor = victim:GetBloodColor()
		end
		
		local oldabsorption = gellyx.presets.getActivePreset().Material.Absorption
		local material = gellyx.presets.getActivePreset().Material

		if bloodcolor == 0 then -- the blood is red >>> ALSO WHEN IT'S RAGDOLL SINCE THEY DON'T HAVE BLOOD COLOR <<<
			material.Absorption = Vector(0.05, 0.5, 0.5)
		elseif bloodcolor == 2 or bloodcolor == 4 then  -- the blood is yellow
			material.Absorption = Vector(0.25, 0.3, 0.5)
		else 											-- the blood is green
			material.Absorption = Vector(0.3, 0.25, 0.5)
		end 

		gelly.SetFluidMaterial(material)

		sprayBlood(type, attacker, position, force, damage)

		material.Absorption = oldabsorption

		gelly.SetFluidMaterial(material)
	end
)

gellyx.presets.select("Blood")
