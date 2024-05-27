return {
	Name = "Goop",
	Radius = 5,
	SolverParams = {
		Viscosity = 20,
		SurfaceTension = 0.0001,
		Adhesion = 0.2,
		Cohesion = 0.2,
		VorticityConfinement = 0,
		DynamicFriction = 0.1,
	},
	Material = {
		Roughness = 1, -- Goop is shiny
		IsSpecularTransmission = false, -- Goop is translucent (although we definitely could make it diffuse, but slime is usually translucent)
		RefractiveIndex = 1.333, -- Goop has a refractive index similar to water
		Absorption = Vector(0, 0.005, 0),
	},
}
