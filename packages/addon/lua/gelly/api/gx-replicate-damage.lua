gellyx = gellyx or {}
gellyx.DAMAGE_REP_NETMSG = "GellyReplicateDamage"

if SERVER then
	--- Replicates damage to the client.
	--- Workaround for the fact that the damage hook doesn't work clientside.
	---@param victim Entity
	---@param attacker Entity
	---@param position Vector
	---@param force Vector
	---@param damage number
	---@param type number
	function gellyx.replicateDamage(victim, attacker, position, force, damage, type)
		net.Start(gellyx.DAMAGE_REP_NETMSG)
		net.WriteEntity(victim)
		net.WriteEntity(attacker)
		net.WriteVector(position)
		net.WriteVector(force)
		net.WriteFloat(damage)
		net.WriteUInt(type, 32)
		net.Broadcast()
	end
end

if CLIENT then
	net.Receive(gellyx.DAMAGE_REP_NETMSG, function()
		local victim = net.ReadEntity()
		local attacker = net.ReadEntity()
		local position = net.ReadVector()
		local force = net.ReadVector()
		local damage = net.ReadFloat()
		local type = net.ReadUInt(32)

		hook.Run("GellyXDamage", victim, attacker, position, force, damage, type)
	end)
end
