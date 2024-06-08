return {
	Name = "Water",
	Radius = 3,
	DiffuseScale = 0.25,
	SolverParams = {
		Viscosity = 0,
		SurfaceTension = 0,
		Adhesion = 0,
		Cohesion = 0.01,
		VorticityConfinement = 10,
		DynamicFriction = 0.1,
	},
	Material = {
		Roughness = 0,
		IsSpecularTransmission = true,
		RefractiveIndex = 1.333,
		Absorption = Vector(0.01, 0.01, 0.005),
		DiffuseColor = Vector(0, 0, 0),
	},
}
