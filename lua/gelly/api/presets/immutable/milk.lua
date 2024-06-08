return {
	Name = "Milk",
	Radius = 3,
	DiffuseScale = 0,
	SolverParams = {
		Viscosity = 20,
		SurfaceTension = 0.001,
		Adhesion = 0,
		Cohesion = 0.2,
		VorticityConfinement = 0,
		DynamicFriction = 0.1,
	},
	Material = {
		Roughness = 0,
		IsSpecularTransmission = true,
		RefractiveIndex = 1.333,
		Absorption = Vector(-0.03, -0.03, -0.03),
		DiffuseColor = Vector(0, 0, 0),
	},
}
