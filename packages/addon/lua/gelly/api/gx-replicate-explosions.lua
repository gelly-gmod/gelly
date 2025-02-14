gellyx = gellyx or {}
gellyx.EXPLOSION_NETMSG = "GellyReplicateExplosion"

if SERVER then
	util.AddNetworkString(gellyx.EXPLOSION_NETMSG)
	hook.Add("OnEntityCreated", "gellyx.replicate-explosion", function(ent)
		timer.Simple(0, function() -- delay til its internal variables are set
			if ent and IsValid(ent) and ent:GetClass() == "env_explosion" then
				net.Start(gellyx.EXPLOSION_NETMSG)
				net.WriteVector(ent:GetPos())
				net.WriteFloat(ent:GetInternalVariable("iMagnitude"))
				net.Broadcast()
			end
		end)
	end)

	hook.Add("EntityRemoved", "gellyx.replicate-explosion", function(ent)
		if ent and IsValid(ent) and ent:GetClass() == "npc_grenade_frag" then
			net.Start(gellyx.EXPLOSION_NETMSG)
			net.WriteVector(ent:GetPos())
			net.WriteFloat(40) -- default magnitude
			net.Broadcast()
		end
	end)

	return
end

net.Receive(gellyx.EXPLOSION_NETMSG, function()
	local pos = net.ReadVector()
	local magnitude = net.ReadFloat()
	hook.Run("GellyXExplosion", pos, magnitude)
end)
