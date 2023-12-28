return {
	Name = "Water",
	SolverParams = {
		Viscosity = 0,
		SurfaceTension = 0,
		Adhesion = 0,
		Cohesion = 0.005,
		VorticityConfinement = 0,
	},
	VisualParams = {
		-- phong shininess
		Shininess = 200,
		Absorption = Vector(0.03, 0.01, 0.01),
		RefractionStrength = 0.015,
	},
}
