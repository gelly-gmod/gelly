return {
	Name = "Goop",
	SolverParams = {
		Viscosity = 80,
		SurfaceTension = 0,
		Adhesion = 0.2,
		Cohesion = 1.5,
		VorticityConfinement = 0,
	},
	VisualParams = {
		-- phong shininess
		Shininess = 400000,
		Absorption = Vector(0.01, 0.01, 0),
		RefractionStrength = 0.008,
	},
}
