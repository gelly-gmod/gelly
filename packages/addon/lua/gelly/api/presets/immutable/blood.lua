return {
	Name = "Blood",
	Radius = 3.50,
	SolverParams = {
		Viscosity = 0,
		SurfaceTension = 0.1,
		Adhesion = 0.05,
		Cohesion = 0.05,
		VorticityConfinement = 0,
		DynamicFriction = 1.1,
		RestDistanceRatio = 0.55
	},
	Material = {
		Roughness = 0,           -- blood isn't rough at all
		IsSpecularTransmission = true, -- blood is translucent
		RefractiveIndex = 1.373, -- blood has a slightly higher refractive index than water
		Absorption = Vector(0.3, 1.1, 1.1) * 40,
		DiffuseColor = Vector(0, 0, 0),
	},
}
