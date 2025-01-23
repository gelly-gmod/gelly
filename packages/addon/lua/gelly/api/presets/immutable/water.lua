return {
	Name = "Water",
	Radius = 3.5,
	UseWhitewater = true,
	SolverParams = {
		Viscosity = 0,
		SurfaceTension = 0.04,
		Adhesion = 0,
		Cohesion = 0.05,
		VorticityConfinement = 15,
		DynamicFriction = 1,
		RestDistanceRatio = 0.60,
	},
	Material = {
		Roughness = 0,
		IsSpecularTransmission = true,
		RefractiveIndex = 1.333,
		Absorption = Vector(0.1, 0.03, 0.02) * 10,
		DiffuseColor = Vector(0, 0, 0),
	},
}
