return {
	Name = "Blood",
	Radius = 2.50,
	SolverParams = {
		Viscosity = 25,
		SurfaceTension = 0.00,
		Adhesion = 0.3,
		Cohesion = 0.01,
		VorticityConfinement = 0,
		DynamicFriction = 0.1,
	},
	Material = {
		Roughness = 0, -- blood isn't rough at all
		IsSpecularTransmission = true, -- blood is translucent
		RefractiveIndex = 1.373, -- blood has a slightly higher refractive index than water
		Absorption = Vector(0.05, 0.5, 0.5),
	},
}
