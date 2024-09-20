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
	fluid_radius = settingConvar("fluid_radius", 1, "Controls the radius of the fluid", 1, 5),
	fluid_viscosity = settingConvar("fluid_viscosity", 1, "Controls the viscosity of the fluid", 0, 100),
	fluid_cohesion = settingConvar("fluid_cohesion", 1, "Controls the cohesion of the fluid", 0, 1),
	fluid_adhesion = settingConvar("fluid_adhesion", 1, "Controls the adhesion of the fluid", 0, 1),
	fluid_friction = settingConvar("fluid_friction", 1, "Controls the friction of the fluid", 0, 10),
	fluid_rest_distance_ratio = settingConvar("fluid_rest_distance_ratio", 1,
		"Controls the rest distance ratio of the fluid", 0, 1),
	fluid_color_hex = settingConvar("fluid_color_hex", "", "Controls the color of the fluid"),
	fluid_color_scale = settingConvar("fluid_color_scale", 1, "Controls the color scale of the fluid", 0, 10),
	fluid_roughness = settingConvar("fluid_roughness", 1, "Controls the roughness of the fluid", 0, 1),
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
