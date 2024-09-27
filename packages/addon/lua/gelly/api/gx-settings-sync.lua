local logging = include("gelly/logging.lua")

-- Synchronizes the binary module with the settings
gellyx = gellyx or {}
assert(gellyx.settings, "Cannot synchronize settings without settings module loaded")

local BINARY_MODULE_RELATED_SETTINGS = {
	"smoothness",
	"simulation_substeps",
	"simulation_iterations",
	"max_particles"
}

function gellyx.settings.updateBinaryModuleSettings(changedConvar)
	-- The binary module's settings are ephemeral, which is why we synchronize them with the convars
	gelly.SetGellySettings({
		FilterIterations = gellyx.settings.get("smoothness"):GetInt(),
		EnableGPUSynchronization = true
	})

	gelly.ConfigureSim({
		Substeps = gellyx.settings.get("simulation_substeps"):GetInt(),
		Iterations = gellyx.settings.get("simulation_iterations"):GetInt(),
	})

	if changedConvar == nil or changedConvar == gellyx.settings.getFullName("max_particles") then
		-- This is one of our most expensive changes so we ensure it's only done when necessary
		-- But on initialization, we need to set it again so we check if there even was a changed convar.

		gelly.ChangeMaxParticles(gellyx.settings.get("max_particles"):GetInt())
		gellyx.presets.select(gellyx.presets.getActivePreset().Name)
		hook.Run("GellyRestarted")

		logging.warn("Max particles set to " .. gellyx.settings.get("max_particles"):GetInt() .. "!")
	end
end

gellyx.settings.registerMultipleOnChange(BINARY_MODULE_RELATED_SETTINGS, gellyx.settings.updateBinaryModuleSettings)
