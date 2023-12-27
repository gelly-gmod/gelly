return {
	Name = "Water",
	SolverParams = {
		Viscosity = 0,
		SurfaceTension = 1,
		Adhesion = 0,
		Cohesion = 0.025,
		VorticityConfinement = 2,
	},
	VisualParams = {
		-- phong shininess
		Shininess = 900,
		Absorption = Vector(0.1, 0.1, 0),
		RefractionStrength = 0.01,
	},
}
