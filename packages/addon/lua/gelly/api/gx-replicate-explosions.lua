gellyx = gellyx or {}
gellyx.EXPLOSION_NETMSG = "GellyReplicateExplosion"

local GRENADE_DETONATE_THRESHOLD = 0.1

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

	local detonatedCache = {}
	hook.Add("Think", "gellyx.detect-grenade-explosions", function()
		for _, grenade in ipairs(ents.FindByClass("npc_grenade_frag")) do
			local detonateTime = grenade:GetInternalVariable("m_flDetonateTime")
			if detonateTime <= GRENADE_DETONATE_THRESHOLD and not detonatedCache[grenade] then
				net.Start(gellyx.EXPLOSION_NETMSG)
				net.WriteVector(grenade:GetPos())
				net.WriteFloat(15)
				net.Broadcast()

				detonatedCache[grenade] = true
			end
		end
	end)

	hook.Add("EntityRemoved", "gellyx.detect-grenade-explosions", function(ent)
		if ent:GetClass() == "grenade_ar2" then
			net.Start(gellyx.EXPLOSION_NETMSG)
			net.WriteVector(ent:GetPos())
			net.WriteFloat(15)
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
