return {
	Name = "Piss",
	Radius = 3.5,
	UseWhitewater = true,
	SolverParams = {
		Viscosity = 0,
		SurfaceTension = 0.04,
		Adhesion = 0,
		Cohesion = 0.05,
		VorticityConfinement = 10,
		DynamicFriction = 1,
		RestDistanceRatio = 0.60,
	},
	Material = {
		-- Pee is pretty much water, so we can just copy the water material
		Roughness = 0,
		IsSpecularTransmission = true,
		RefractiveIndex = 1.333,
		Absorption = Vector(0.03, 0.03, 0.2) * 20,
		DiffuseColor = Vector(0, 0, 0),
	},
}
