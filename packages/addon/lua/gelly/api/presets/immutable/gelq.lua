return {
	Name = "Gelq",
	Color = Color(169, 42, 212, 255),
	Radius = 3,
	SolverParams = {
		Viscosity = 10,
		SurfaceTension = 0.25,
		Adhesion = 0.0,
		Cohesion = 0.13,
		VorticityConfinement = 0,
		DynamicFriction = 0.1,
		RestDistanceRatio = 0.55,
	},
	Material = {
		Roughness = 0,
		IsSpecularTransmission = false,
		RefractiveIndex = 1.333,
		Absorption = Vector(0.02, 0.1, 0.1) * 320,
		DiffuseColor = Vector(0.66, 0.16, 0.83)
	},
}
