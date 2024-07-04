return {
	Name = "Water",
	Radius = 3.5,
	DiffuseScale = 0.25,
	SolverParams = {
		Viscosity = 0,
		SurfaceTension = 0.00005,
		Adhesion = 0,
		Cohesion = 0.01,
		VorticityConfinement = 15,
		DynamicFriction = 1,
	},
	Material = {
		Roughness = 0,
		IsSpecularTransmission = true,
		RefractiveIndex = 1.333,
		Absorption = Vector(0.1, 0.03, 0.02),
		DiffuseColor = Vector(0, 0, 0),
	},
}
