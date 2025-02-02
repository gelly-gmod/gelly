return {
	Name = "Glunk",
	Color = Color(255, 255, 255, 255),
	Radius = 3,
	SolverParams = {
		Viscosity = 20,
		SurfaceTension = 0,
		Adhesion = 0,
		Cohesion = 0.8,
		VorticityConfinement = 0,
		DynamicFriction = 0.1,
		RestDistanceRatio = 0.58,
	},
	Material = {
		Roughness = 1,
		IsSpecularTransmission = false,
		RefractiveIndex = 1.333,      -- Glunk has a refractive index similar to water
		Absorption = Vector(100, 100, 100), -- Glunk is opaque
		DiffuseColor = Vector(1, 1, 1),
	},
}
