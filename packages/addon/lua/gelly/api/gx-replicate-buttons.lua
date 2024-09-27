gellyx = gellyx or {}
gellyx.BUTTON_NETMSG = "GellyReplicateButton"

if SERVER then
	util.AddNetworkString(gellyx.BUTTON_NETMSG)
	hook.Add("PlayerButtonDown", "gellyx.replicate-button-down", function(ply, key)
		net.Start(gellyx.BUTTON_NETMSG)
		net.WriteInt(key, 32)
		net.Broadcast()
	end)
	return
end

net.Receive(gellyx.BUTTON_NETMSG, function()
	local key = net.ReadInt(32)
	hook.Run("GellyXButtonDown", key)
end)
