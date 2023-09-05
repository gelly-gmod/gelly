require("gelly")

normal_rt, depth_rt, gellysim = gelly.Create(1000000, 2, ScrW(), ScrH())

gellysim:LoadMap(game.GetMap())

local viewSetup = render.GetViewSetup(true)
gellysim:SetupCamera(viewSetup.fov, ScrW(), ScrH(), viewSetup.znear, viewSetup.zfar)

local particleCount = 0
-- Used to prevent the simulation from having uncontrollable numerical error when the delta time spikes
local MAX_DELTATIME = 1 / 60

hook.Add("PreDrawOpaqueRenderables", "gelly_test.render_simulate", function(isDrawDepth, isSkybox)
	local eyeDirection = LocalPlayer():GetAimVector()
	local eyePos = LocalPlayer():EyePos()

	gellysim:SyncCamera(eyePos, eyeDirection)
	-- Speed multiplier just to make the simulation more interesting right now.
	gellysim:Update(math.min(FrameTime(), MAX_DELTATIME) * 6)

	for i = 1, 4300 do
		if not input.IsKeyDown(KEY_R) then break end

		local offset = VectorRand() * 12
		gellysim:AddParticle(eyePos + offset, (LocalPlayer():GetAimVector() + Vector(math.Rand(-0.1, 0.1), math.Rand(-0.1, 0.1), math.Rand(-0.1, 0.1))) * 150)
		particleCount = particleCount + 1
	end

	gellysim:Render()
	gellysim:Composite()
end)

hook.Add("HUDPaint", "gelly_test.stats", function ()
	draw.SimpleText(("Active particle count: %d"):format(particleCount), "ChatFont", 15, 15, Color(255, 255, 255))
	draw.SimpleText(("Frame duration: %.2fms"):format(FrameTime() * 1000), "ChatFont", 15, 30, Color(255, 255, 255))
	draw.SimpleText(("Active feature: %s"):format(drawNormal and "Normals" or "Depth"), "ChatFont", 15, 45, Color(255, 255, 255))
end)

local lastTimePressed = 0
local cooldownSeconds = 3

hook.Add("Think", "gelly_test.clear", function ()
	if input.IsKeyDown(KEY_M) and (SysTime() - lastTimePressed > cooldownSeconds) then
		gellysim:Clear()
		particleCount = 0
		lastTimePressed = SysTime()
	end
end)

