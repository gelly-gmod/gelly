local logging = include("gelly/logging.lua")

-- Synchronizes the binary module with the settings
gellyx = gellyx or {}
assert(gellyx.settings, "Cannot synchronize settings without settings module loaded")

local BINARY_MODULE_RELATED_SETTINGS = {
	"smoothness",
	"simulation_substeps",
	"simulation_iterations",
	"simulation_relaxation",
	"simulation_collision_distance",
	"simulation_gravity",
	"spray_scale",
	"spray_motion_blur",
	"spray_lifetime",
	"spray_threshold",
	"spray_buoyancy",
	"spray_ballistic",
	"spray_drag",
	"whitewater_enabled",
	"whitewater_strength",
	"resolution_scale",
	"glunk_lighting_fix",
	"max_particles"
}

function gellyx.settings.updateBinaryModuleSettings(changedConvar)
	if changedConvar == nil or changedConvar == gellyx.settings.getFullName("max_particles") then
		-- This is one of our most expensive changes so we ensure it's only done when necessary
		-- But on initialization, we need to set it again so we check if there even was a changed convar.

		gelly.ChangeMaxParticles(gellyx.settings.get("max_particles"):GetInt())
		gellyx.presets.select(gellyx.presets.getActivePreset().Name)
		hook.Run("GellyRestarted")

		logging.warn("Max particles set to " .. gellyx.settings.get("max_particles"):GetInt() .. "!")
	end

	if changedConvar == nil or changedConvar == gellyx.settings.getFullName("resolution_scale") then
		-- Thankfully the sim and renderer are decoupled enough such that we dont
		-- need to restart Gelly in this case
		gelly.ChangeResolution(
			ScrW(),
			ScrH(),
			gellyx.settings.get("resolution_scale"):GetFloat()
		)

		logging.warn("Rendering at %dx%d with a scale of %.2f!", ScrW(), ScrH(),
			gellyx.settings.get("resolution_scale"):GetFloat())
	end

	gelly.SetGellySettings({
		FilterIterations = gellyx.settings.get("smoothness"):GetInt(),
		EnableGPUSynchronization = true,
		EnableWhitewater = gellyx.settings.get("whitewater_enabled"):GetBool() and
			gellyx.presets.isPresetUsingWhitewater(),
		WhitewaterStrength = gellyx.settings.get("whitewater_strength"):GetFloat(),
	})

	gelly.ConfigureSim({
		Substeps = gellyx.settings.get("simulation_substeps"):GetInt(),
		Iterations = gellyx.settings.get("simulation_iterations"):GetInt(),
		RelaxationFactor = gellyx.settings.get("simulation_relaxation"):GetFloat(),
		CollisionDistance = gellyx.settings.get("simulation_collision_distance"):GetFloat(),
		Gravity = gellyx.settings.get("simulation_gravity"):GetFloat(),
		EnableWhitewater = gellyx.settings.get("whitewater_enabled"):GetBool()
	})

	local isLightingFixEnabled = gellyx.settings.get("glunk_lighting_fix"):GetBool()

	if isLightingFixEnabled then
		RunConsoleCommand("r_worldlights", "0")
	else
		RunConsoleCommand("r_worldlights", "4")
	end

	gelly.SetDiffuseProperties({
		BallisticCount = gellyx.settings.get("spray_ballistic"):GetInt(),
		KineticThreshold = gellyx.settings.get("spray_threshold"):GetInt(),
		Drag = gellyx.settings.get("spray_drag"):GetFloat(),
		Buoyancy = gellyx.settings.get("spray_buoyancy"):GetFloat(),
		Lifetime = gellyx.settings.get("spray_lifetime"):GetFloat(),
	})

	gelly.SetDiffuseScale(gellyx.settings.get("spray_scale"):GetFloat() *
		gellyx.settings.get("preset_radius_scale"):GetFloat())
	gelly.SetDiffuseMotionBlur(gellyx.settings.get("spray_motion_blur"):GetFloat())
end

gellyx.settings.registerMultipleOnChange(BINARY_MODULE_RELATED_SETTINGS, gellyx.settings.updateBinaryModuleSettings)
