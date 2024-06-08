return {
	Name = "Piss",
	Radius = 5,
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
		-- Pee is pretty much water, so we can just copy the water material
		Roughness = 0,
		IsSpecularTransmission = true,
		RefractiveIndex = 1.333,
		Absorption = Vector(0.0, 0.0, 0.01),
		DiffuseColor = Vector(0, 0, 0),
	},
}
