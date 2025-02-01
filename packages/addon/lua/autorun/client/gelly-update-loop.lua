local logging = include("gelly/logging.lua")
local ENVBALLS_MODEL_PATH = "models/shadertest/envballs.mdl"

GELLY_SIM_TIMESCALE = 10
GELLY_SIM_RATE_HZ = 60
SIMULATE_GELLY = true

local lastTimescale = GELLY_SIM_TIMESCALE
local lastRate = GELLY_SIM_RATE_HZ
local lastSimTime = SysTime()

local function isGellyActive()
	return gelly.GetStatus().ActiveParticles > 0
end

hook.Add("GellyLoaded", "gelly.update-loop", function()
	-- forces the engine to compute cubemaps for us
	local envballsModel = ClientsideModel(ENVBALLS_MODEL_PATH)

	-- by this point GellyX has loaded already
	gellyx.settings.registerOnChange("simulation_rate", function()
		GELLY_SIM_RATE_HZ = gellyx.settings.get("simulation_rate"):GetInt()
	end)

	GELLY_SIM_RATE_HZ = gellyx.settings.get("simulation_rate"):GetInt()

	local simulationStarted = false

	hook.Add("PreRender", "gelly.simulate", function()
		if lastRate ~= GELLY_SIM_RATE_HZ then
			lastRate = GELLY_SIM_RATE_HZ
		end

		if SIMULATE_GELLY then
			if lastTimescale ~= GELLY_SIM_TIMESCALE then
				lastTimescale = GELLY_SIM_TIMESCALE
				gelly.SetTimeStepMultiplier(GELLY_SIM_TIMESCALE)
			end

			local now = SysTime()
			local dt = now - lastSimTime

			if dt >= 1 / GELLY_SIM_RATE_HZ and simulationStarted then
				gelly.EndTick()
				simulationStarted = false
			end
		end
	end)

	local lastScrW = ScrW()
	local lastScrH = ScrH()
	hook.Add("RenderScene", "gelly.render", function()
		if lastScrW ~= ScrW() or lastScrH ~= ScrH() then
			lastScrW = ScrW()
			lastScrH = ScrH()
			gelly.ChangeResolution(ScrW(), ScrH(), gellyx.settings.get("resolution_scale"):GetFloat())
			GELLY_CUSTOMIZATION:AdjustSize()

			logging.warn("Rendering at %dx%d with a scale of %.2f! (detected resolution change)", ScrW(), ScrH(),
				gellyx.settings.get("resolution_scale"):GetFloat())
		end

		if isGellyActive() then
			gelly.StartRendering()
		end

		if SIMULATE_GELLY then
			local now = SysTime()
			local dt = now - lastSimTime

			if dt >= 1 / GELLY_SIM_RATE_HZ then
				lastSimTime = now
				simulationStarted = true
				local maxCompensation = (1 / GELLY_SIM_RATE_HZ) * 1.15 -- maximum overstep allowed
				gelly.BeginTick(math.min(dt, maxCompensation))
			end
		end
	end)

	hook.Add("PostDrawOpaqueRenderables", "gelly.composite", function()
		if not isGellyActive() then
			envballsModel:SetPos(Vector(0, 0, -1e5))
			return
		end

		render.Model({
			model = ENVBALLS_MODEL_PATH,
			pos = EyePos() - EyeAngles():Forward() * 40,
			angle = EyeAngles(),
		}, envballsModel)
		envballsModel:SetNoDraw(true)

		gelly.EndRendering()
		gelly.Composite()
	end)
end)
