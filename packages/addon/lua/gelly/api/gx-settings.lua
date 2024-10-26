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
	simulation_relaxation = settingConvar("simulation_relaxation", 1, "Controls the relaxation factor of the solver", 0,
		1),
	simulation_collision_distance = settingConvar("simulation_collision_distance", 0.6,
		"Controls the collision distance of the fluid", 0.01, 1),
	simulation_gravity = settingConvar("simulation_gravity", -4, "Controls the gravity of the fluid", -100, 100),

	fluid_radius = settingConvar("fluid_radius", 1, "Controls the radius of the fluid", 1, 5),
	fluid_viscosity = settingConvar("fluid_viscosity", 1, "Controls the viscosity of the fluid", 0, 100),
	fluid_cohesion = settingConvar("fluid_cohesion", 1, "Controls the cohesion of the fluid", 0, 1),
	fluid_adhesion = settingConvar("fluid_adhesion", 1, "Controls the adhesion of the fluid", 0, 1),
	fluid_friction = settingConvar("fluid_friction", 1, "Controls the friction of the fluid", 0, 10),
	fluid_rest_distance_ratio = settingConvar("fluid_rest_distance_ratio", 0.65,
		"Controls the rest distance ratio of the fluid", 0.5, 0.7),
	fluid_color_hex = settingConvar("fluid_color_hex", "", "Controls the color of the fluid"),
	fluid_color_scale = settingConvar("fluid_color_scale", 1, "Controls the color scale of the fluid", 0, 10),
	fluid_roughness = settingConvar("fluid_roughness", 1, "Controls the roughness of the fluid", 0, 1),
	fluid_opaque = settingConvar("fluid_opaque", 0, "Controls the opacity of the fluid (0 = transparent, 1 = opaque)", 0,
		1),
	fluid_use_whitewater = settingConvar("fluid_use_whitewater", 1,
		"Controls the visibility of whitewater for the ephemeral fluid preset (0 = off, 1 = on)",
		0, 1),

	spray_scale = settingConvar("spray_scale", 0.3, "Controls the scale of spray particles.", 0, 4),
	spray_motion_blur = settingConvar("spray_motion_blur", 4.5, "Controls the motion blur of spray particles.", 0, 10),
	spray_lifetime = settingConvar("spray_lifetime", 0.5, "Controls the lifetime of spray particles.", 0, 10),
	spray_threshold = settingConvar("spray_threshold", 30, "Controls the threshold of spray particles.", 0, 250),
	spray_buoyancy = settingConvar("spray_buoyancy", 1.0, "Controls the buoyancy of spray particles.", 0, 1),
	spray_ballistic = settingConvar("spray_ballistic", 0,
		"Controls the ballistic threshold of spray particles (it is best to leave this alone).", 0, 32),
	spray_drag = settingConvar("spray_drag", 0.2, "Controls the drag of spray particles.", 0, 3),
	whitewater_enabled = settingConvar("whitewater_enabled", 1, "Controls the visibility of whitewater (0 = off, 1 = on)",
		0, 1),

	max_particles = settingConvar("max_particles", 100000, "Controls the maximum number of particles (expensive!)", 10000,
		1500000),
	sun_visible = settingConvar("sun_visible", 1, "Controls the visibility of the sun (0 = no sun, 1 = sun)", 0, 1),

	gelly_gun_forcefield_strength = settingConvar("gelly_gun_forcefield_strength", -100,
		"Controls the strength of the forcefield", -1000, 1000),
	gelly_gun_forcefield_radius = settingConvar("gelly_gun_forcefield_radius", 100,
		"Controls the radius of the forcefield", 1, 1000),
	gelly_gun_distance = settingConvar("gelly_gun_distance", 110, "Controls the distance of the emission from the camera",
		10, 400),
	gelly_gun_primary_size = settingConvar("gelly_gun_primary_size", 5, "Controls the size of the primary emission", 1,
		10),
	gelly_gun_secondary_size = settingConvar("gelly_gun_secondary_size", 5, "Controls the size of the secondary emission",
		1, 10),
	gelly_gun_secondary_velocity = settingConvar("gelly_gun_secondary_velocity", 70,
		"Controls the velocity of the secondary emission", 10, 220),
	gelly_gun_density = settingConvar("gelly_gun_density", 150, "Controls the density of the emission", 1, 1000),

	resolution_scale = settingConvar("resolution_scale", 0.75,
		"Controls the rendering scale of the fluid as a percentage of the current resolution.", 0.25, 1.25),
	glunk_lighting_fix = settingConvar("glunk_lighting_fix", 1, "Controls the lighting fix for Glunk (0 = off, 1 = on)",
		0, 1),
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

function gellyx.settings.getFullName(name)
	return gellyx.settings.key .. name
end
