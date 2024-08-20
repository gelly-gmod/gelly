return {
	Name = "Blood",
	Radius = 3.50,
	DiffuseScale = 0,
	SolverParams = {
		Viscosity = 0,
		SurfaceTension = 0.0001,
		Adhesion = 0.0009,
		Cohesion = 0.1,
		VorticityConfinement = 0,
		DynamicFriction = 1.1,
	},
	Material = {
		Roughness = 0,           -- blood isn't rough at all
		IsSpecularTransmission = true, -- blood is translucent
		RefractiveIndex = 1.373, -- blood has a slightly higher refractive index than water
		Absorption = Vector(0.3, 1.1, 1.1) * 4,
		DiffuseColor = Vector(0, 0, 0),
	},
}
