gellyx = gellyx or {}
gellyx.BULLET_NETMSG = "GellyReplicateBullet"

if SERVER then
	util.AddNetworkString(gellyx.BULLET_NETMSG)
	hook.Add("PostEntityFireBullets", "gellyx.replicate-bullet", function(_, bullet)
		net.Start(gellyx.BULLET_NETMSG)
		net.WriteVector(bullet.Trace.StartPos)
		net.WriteVector(bullet.Trace.HitPos)
		net.Broadcast()
	end)
	return
end

net.Receive(gellyx.BULLET_NETMSG, function()
	local startPos = net.ReadVector()
	local endPos = net.ReadVector()
	hook.Run("GellyXBulletFired", startPos, endPos)
end)
