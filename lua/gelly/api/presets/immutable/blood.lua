return {
	Name = "Blood",
	Radius = 1.10,
	SolverParams = {
		Viscosity = 0,
		SurfaceTension = 0.003,
		Adhesion = 0.01,
		Cohesion = 0.01,
		VorticityConfinement = 0,
		DynamicFriction = 1,
	},
	VisualParams = {
		-- phong shininess
		Shininess = 200,
		Absorption = Vector(0.01, 0.5, 0.5),
		RefractionStrength = 0.005,
	},
}
