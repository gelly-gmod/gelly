-- Synchronizes the binary module with the settings
gellyx = gellyx or {}
assert(gellyx.settings, "Cannot synchronize settings without settings module loaded")

local BINARY_MODULE_RELATED_SETTINGS = {
	"smoothness",
	"simulation_substeps",
	"simulation_iterations",
}

function gellyx.settings.updateBinaryModuleSettings()
	-- The binary module's settings are ephemeral, which is why we synchronize them with the convars
	gelly.SetGellySettings({
		FilterIterations = gellyx.settings.get("smoothness"):GetInt(),
		EnableGPUSynchronization = true
	})

	gelly.ConfigureSim({
		Substeps = gellyx.settings.get("simulation_substeps"):GetInt(),
		Iterations = gellyx.settings.get("simulation_iterations"):GetInt(),
	})
end

gellyx.settings.registerMultipleOnChange(BINARY_MODULE_RELATED_SETTINGS, gellyx.settings.updateBinaryModuleSettings)
