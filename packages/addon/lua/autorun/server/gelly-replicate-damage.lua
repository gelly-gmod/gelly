---@module "gelly.api.gx-replicate-damage"
include("gelly/api/gx-replicate-damage.lua")

util.AddNetworkString(gellyx.DAMAGE_REP_NETMSG)

hook.Add("EntityTakeDamage", "gelly.damage-replication", function(target, dmgInfo)
	local attacker = dmgInfo:GetAttacker()
	local damage = dmgInfo:GetDamage()
	local position = dmgInfo:GetDamagePosition()
	local force = dmgInfo:GetDamageForce()
	local type = dmgInfo:GetDamageType()

	print(target.BloodQuantity)

	if target.BloodQuantity > 0 then
		gellyx.replicateDamage(target, attacker, position, force, damage * (target.BloodQuantity / 100), type) -- this makes the blood amout expelled lower with lowering the blood quantity
		target.BloodQuantity = target.BloodQuantity - dmgInfo:GetDamage() * 0.25 -- scale damage to be more 🤌
	end
end)
