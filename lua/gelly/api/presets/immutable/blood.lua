return {
	Name = "Blood",
	SolverParams = {
		Viscosity = 5,
		SurfaceTension = 0,
		Adhesion = 0.001,
		Cohesion = 0.05,
		VorticityConfinement = 10,
		DynamicFriction = 2,
	},
	VisualParams = {
		-- phong shininess
		Shininess = 200,
		Absorption = Vector(0.01, 0.5, 0.5),
		RefractionStrength = 0.005,
	},
}
