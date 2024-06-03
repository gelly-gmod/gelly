return {
	Name = "Glunk",
	Radius = 3,
	DiffuseScale = 0,
	SolverParams = {
		Viscosity = 20,
		SurfaceTension = 0.001,
		Adhesion = 0,
		Cohesion = 0.8,
		VorticityConfinement = 0,
		DynamicFriction = 0.1,
	},
	Material = {
		Roughness = 1,
		IsSpecularTransmission = false,
		RefractiveIndex = 1.333, -- Goop has a refractive index similar to water
		Absorption = Vector(0, 0.005, 0),
	},
}
