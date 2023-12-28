return {
	Name = "Goop",
	SolverParams = {
		Viscosity = 80,
		SurfaceTension = 0,
		Adhesion = 0.2,
		Cohesion = 0.005,
		VorticityConfinement = 0,
	},
	VisualParams = {
		-- phong shininess
		Shininess = 200,
		Absorption = Vector(2, 0, 2),
		RefractionStrength = 0.012,
	},
}
