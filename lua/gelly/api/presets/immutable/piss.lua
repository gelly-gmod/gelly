return {
	Name = "Piss",
	SolverParams = {
		Viscosity = 0,
		SurfaceTension = 0,
		Adhesion = 0,
		Cohesion = 0.005,
		VorticityConfinement = 10,
	},
	VisualParams = {
		-- phong shininess
		Shininess = 200,
		Absorption = Vector(0, 0, 0.5) * 0.005,
		RefractionStrength = 0.015,
	},
}
