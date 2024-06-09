---@module "gelly.api.gx-replicate-damage"
include("gelly/api/gx-replicate-damage.lua")

util.AddNetworkString(gellyx.DAMAGE_REP_NETMSG)

hook.Add("EntityTakeDamage", "gelly.damage-replication", function(target, dmgInfo)
	local attacker = dmgInfo:GetAttacker()
	local damage = dmgInfo:GetDamage()
	local position = dmgInfo:GetDamagePosition()
	local force = dmgInfo:GetDamageForce()
	local type = dmgInfo:GetDamageType()

	gellyx.replicateDamage(target, attacker, position, force, damage, type)
end)
