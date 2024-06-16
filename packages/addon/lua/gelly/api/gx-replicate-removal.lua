gellyx = gellyx or {}
gellyx.REMOVAL_NETMSG = "GellyReplicateRemoval"

if SERVER then
	util.AddNetworkString(gellyx.REMOVAL_NETMSG)

	net.Receive(gellyx.REMOVAL_NETMSG, function()
		local ent = net.ReadEntity()
		if not IsValid(ent) then return end

		ent:Remove()
	end)

	return
end

function gellyx.removeEntity(ent)
	if not IsValid(ent) then return end

	net.Start(gellyx.REMOVAL_NETMSG)
	net.WriteEntity(ent)
	net.SendToServer()
end
