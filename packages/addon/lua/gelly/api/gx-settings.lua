gellyx = gellyx or {}
gellyx.settings = gellyx.settings or {}
gellyx.settings.key = "gelly_"

local function settingConvar(name, default, help, min, max)
	return CreateClientConVar(gellyx.settings.key .. name, default, true, false, help, min, max)
end

gellyx.settings.convars = {
	smoothness = settingConvar("smoothness", 5, "Controls the smoothness of the fluid", 1, 20),
	preset_radius_scale = settingConvar("preset_radius_scale", 1, "Controls the radius scale of the fluid", 1, 5),
	simulation_rate = settingConvar("simulation_rate", 60, "Controls the simulation rate of the fluid", 20, 144),
	simulation_substeps = settingConvar("simulation_substeps", 3, "Controls the substeps of the fluid", 1, 100),
	simulation_iterations = settingConvar("simulation_iterations", 3, "Controls the iterations of the fluid", 1, 100),
}

function gellyx.settings.registerOnChange(name, callback)
	assert(gellyx.settings.convars[name], "Invalid setting name")
	cvars.AddChangeCallback(gellyx.settings.key .. name, callback)
end

function gellyx.settings.registerMultipleOnChange(names, callback)
	for _, name in ipairs(names) do
		gellyx.settings.registerOnChange(name, callback)
	end
end

function gellyx.settings.get(name)
	assert(gellyx.settings.convars[name], "Invalid setting name")
	return gellyx.settings.convars[name]
end
