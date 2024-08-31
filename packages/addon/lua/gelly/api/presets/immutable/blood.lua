return {
	Name = "Blood",
	Radius = 3,
	DiffuseScale = 0,
	SolverParams = {
		Viscosity = 1,
		SurfaceTension = 0.0,
		Adhesion = 0.15,
		Cohesion = 0.45,
		VorticityConfinement = 0,
		DynamicFriction = 0.5,
		RestDistanceRatio = 0.55,
	},
	Material = {
		Roughness = 0,
		IsSpecularTransmission = true,
		RefractiveIndex = 1.333,
		Absorption = Vector(0.02, 0.1, 0.1) * 320,
		DiffuseColor = Vector(0, 0, 0),
	},
}
