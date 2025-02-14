return {
	Name = "Gell-O",
	Color = Color(255, 60, 60, 255),
	Radius = 3,
	SolverParams = {
		Viscosity = 20,
		SurfaceTension = 0.25,
		Adhesion = 0.2,
		Cohesion = 0.2,
		VorticityConfinement = 0,
		DynamicFriction = 0.1,
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
