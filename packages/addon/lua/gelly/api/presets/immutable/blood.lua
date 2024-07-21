return {
	Name = "Blood",
	Radius = 2.50,
	DiffuseScale = 0,
	SolverParams = {
		Viscosity = 25,
		SurfaceTension = 0.0002,
		Adhesion = 0.1,
		Cohesion = 0.2,
		VorticityConfinement = 0,
		DynamicFriction = 1,
	},
	Material = {
		Roughness = 0,           -- blood isn't rough at all
		IsSpecularTransmission = true, -- blood is translucent
		RefractiveIndex = 1.373, -- blood has a slightly higher refractive index than water
		Absorption = Vector(0.5, 1.5, 1.5),
		DiffuseColor = Vector(0, 0, 0),
	},
}

--[[
return {
	Name = "Blood",
	Radius = 2.50,
	DiffuseScale = 0,
	SolverParams = {
		Viscosity = 100.1,
		SurfaceTension = 0.0005,
		Adhesion = 0.2,
		Cohesion = 1.0,
		VorticityConfinement = 0,
		DynamicFriction = 0.9,
	},
	Material = {
		Roughness = 0,           -- blood isn't rough at all
		IsSpecularTransmission = true, -- blood is translucent
		RefractiveIndex = 1.373, -- blood has a slightly higher refractive index than water
		Absorption = Vector(0.3, 1.1, 1.1),
		DiffuseColor = Vector(0, 0, 0),
	},
}
]]

-- Advanced Blood V3,,Color:236 0 0 220/Cohesion:0.150/Adhesion:0.334/Radius:1.00/Gravity:-18.04/Viscosity:10.01/Surface Tension:0.10/Fluid Rest Distance:0.71/Timescale:1.74/Collision Distance:0.14/Dynamic Friction:0.67/Diffuse Threshold:500.0/Diffuse Lifetime:0.0/Vorticity Confinement:0/Anisotropy Scale:1.00/Anisotropy Min:-0.10/Anisotropy Max:2.00