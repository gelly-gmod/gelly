-- One of the benefits of the mod system is that it'll never be ran on the server, so most conditional realm blocks are unnecessary.
print("Blood mod loaded")

local BLOOD_CONFIGS = {
	[DMG_BULLET] = {
		MinDensity = 200,
		MaxDensity = 400,
		VelocityPower = 10, -- bullets usually are rotating so they can end up flinging blood
		Randomness = 0.8, -- spray in the direction of the normal
		CubeSize = 9,
		DamageMultiplier = 15, -- density is added by the damage * this
	},

	[DMG_CRUSH] = {
		MinDensity = 100,
		MaxDensity = 200,
		VelocityPower = 4,
		Randomness = 1,
		CubeSize = 15,
		DamageMultiplier = 20,
	},

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

	[DMG_SLASH] = {
		MinDensity = 100,
		MaxDensity = 200,
		VelocityPower = 4,
		Randomness = 0.1,
		CubeSize = 3,
	},

	[DMG_CLUB] = {
		MinDensity = 100,
		MaxDensity = 200,
		VelocityPower = 4,
		Randomness = 0.1,
		CubeSize = 3,
	},

	[DMG_BLAST] = {
		MinDensity = 3200,
		MaxDensity = 5200,
		VelocityPower = 40,
		Randomness = 1,
		CubeSize = 10,
		DamageMultiplier = 90,
	},
}

local function sprayBlood(damageType, attacker, position, force, damage)
	local normal = force:GetNormalized()
	if attacker == LocalPlayer() then
		normal = LocalPlayer():GetAimVector()
	end

	local config = BLOOD_CONFIGS[damageType]

	if not config then
		return
	end

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
	function(_, attacker, position, force, damage, type)
		if
			not attacker:IsPlayer()
			and not attacker:IsNPC()
			and attacker:GetClass() ~= "prop_ragdoll"
		then
			return
		end
		-- 8194 is the combination used in M9K for bullets
		if type == 8194 then
			type = 2
		end

		print(type)
		sprayBlood(type, attacker, position, force, damage)
	end
)

gellyx.presets.select("Blood")
