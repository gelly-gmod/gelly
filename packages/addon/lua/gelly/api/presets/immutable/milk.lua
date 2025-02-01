return {
	Name = "Milk",
	Color = Color(255, 255, 200, 255),
	Radius = 3,
	SolverParams = {
		Viscosity = 25,
		SurfaceTension = 0.28,
		Adhesion = 0.25,
		Cohesion = 0.25,
		VorticityConfinement = 0,
		DynamicFriction = 0.5,
		RestDistanceRatio = 0.55,
	},
	Material = {
		Roughness = 0,
		IsSpecularTransmission = true,
		IsScatter = true,
		RefractiveIndex = 1.333,
		Absorption = Vector(0.02, 0.1, 0.1) * 320,
		DiffuseColor = Vector(0.2, 0.2, 0.2)
	},
}
