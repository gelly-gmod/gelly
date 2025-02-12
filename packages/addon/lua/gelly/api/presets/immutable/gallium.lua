return {
	Name = "Gallium",
	Color = Color(100, 100, 100, 255),
	Radius = 3.5,
	SolverParams = {
		Viscosity = 0,
		SurfaceTension = 1,
		Adhesion = 0,
		Cohesion = 0.1,
		VorticityConfinement = 15,
		DynamicFriction = 0.1,
		RestDistanceRatio = 0.58,
	},
	Material = {
		Roughness = 0,
		IsSpecularTransmission = false,
		IsMetal = true,
		RefractiveIndex = 1.333,
		Absorption = Vector(0.1, 0.03, 0.02) * 10,
		DiffuseColor = Vector(0.8, 0.8, 0.8),
	},
}
