---@module "gelly.api.gx-replicate-damage"
include("gelly/api/gx-replicate-damage.lua")

util.AddNetworkString(gellyx.DAMAGE_REP_NETMSG)

hook.Add("EntityTakeDamage", "gelly.damage-replication", function(target, dmgInfo)
	local attacker = dmgInfo:GetAttacker()
	local damage = dmgInfo:GetDamage()
	local position = dmgInfo:GetDamagePosition()
	local force = dmgInfo:GetDamageForce()
	local type = dmgInfo:GetDamageType()

	local damagescale = 0.5

	if type == DMG_CRUSH or type == DMG_BLAST then damagescale = 0.02 end

	if target:IsValid() and target.MaxBloodQuantity and target.BloodQuantity > 0 then
		gellyx.replicateDamage(target, attacker, position, force, damage * math.max(0, target.BloodQuantity / target.MaxBloodQuantity), type) -- this makes the blood amout expelled lower with lowering the blood quantity
		target.BloodQuantity = target.BloodQuantity - damage * damagescale -- scale damage to be more 🤌
	end
end)
