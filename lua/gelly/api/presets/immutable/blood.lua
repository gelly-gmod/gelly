return {
	Name = "Blood",
	Radius = 2.50,
	DiffuseScale = 0,
	SolverParams = {
		Viscosity = 0,
		SurfaceTension = 0,
		Adhesion = 0.01,
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
