-- Should never be zero, or the simulation will not run
local FPS_AT_MAX_PARTICLES = 2
START_SIM_FPS = 144

GELLY_SIM_TIMESCALE = 10
GELLY_KILL_PLAYER_ON_CONTACT = false

SIMULATE_GELLY = true

local lastTimescale = GELLY_SIM_TIMESCALE

local frameTimingMs = 0
local lastFrameTime = 0
local frameTimingMs2 = 0
local lastFrameTime2 = 0

local timingHistory1 = {}
local timingHistory2 = {}

local function addTimingHistory(timingHistory, timing)
	table.insert(timingHistory, timing)
	if #timingHistory > 100 then
		table.remove(timingHistory, 1)
	end
end

local function renderGraph(timingHistory, originX, originY, height, width)
	-- X-axis
	surface.SetDrawColor(255, 255, 255)
	surface.DrawLine(originX, originY, originX + width, originY)

	-- Y-axis

	surface.DrawLine(originX, originY, originX, originY - height)

	-- points
	local max = 3
	local xMax = 100

	for i, v in ipairs(timingHistory) do
		v = math.min(v, max)
		local x = originX + (i / xMax) * width
		local y = originY - (v / max) * height
		surface.DrawRect(x, y, 1, 1)
	end

	-- connect points

	for i = 1, #timingHistory - 1 do
		local x1 = originX + (i / xMax) * width
		local y1 = originY - (math.min(timingHistory[i], max) / max) * height
		local x2 = originX + ((i + 1) / xMax) * width
		local y2 = originY - (math.min(timingHistory[i + 1], max) / max) * height
		surface.DrawLine(x1, y1, x2, y2)
	end
end
hook.Add("GellyLoaded", "gelly.update-loop", function()
	timer.Create("gelly.flex-update-timer", 1 / 60, 0, function()
		if SIMULATE_GELLY then
			if lastTimescale ~= GELLY_SIM_TIMESCALE then
				lastTimescale = GELLY_SIM_TIMESCALE
				gelly.SetTimeStepMultiplier(GELLY_SIM_TIMESCALE)
			end

			gelly.Simulate(1 / 60) -- flex is programmed to assume a fixed timestep, normally 60hz
		end
	end)

	hook.Add("RenderScene", "gelly.render", function()
		frameTimingMs = (SysTime() - lastFrameTime) * 1000
		gelly.Render()
		addTimingHistory(timingHistory1, frameTimingMs)
	end)

	hook.Add("PostDrawOpaqueRenderables", "gelly.composite", function(depth, skybox)
		gelly.Composite()
		lastFrameTime = SysTime()
	end)

	hook.Add("PreRender", "gelly.prerender", function()
		frameTimingMs2 = (SysTime() - lastFrameTime2) * 1000
		addTimingHistory(timingHistory2, frameTimingMs2)
	end)

	hook.Add("PostRender", "gelly.postrender", function()
		lastFrameTime2 = SysTime()
	end)

	hook.Add("DrawOverlay", "gelly.timing.hud", function()
		renderGraph(timingHistory1, 10, 100, 100, 200)
		renderGraph(timingHistory2, 10, 350, 100, 200)
	end)
end)
