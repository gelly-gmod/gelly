return {
	Name = "Piss",
	Radius = 3,
	SolverParams = {
		Viscosity = 0,
		SurfaceTension = 0,
		Adhesion = 0,
		Cohesion = 0.01,
		VorticityConfinement = 10,
		DynamicFriction = 0.1,
	},
	VisualParams = {
		-- phong shininess
		Shininess = 200,
		Absorption = Vector(0.0001, 0, 0.0001),
		RefractionStrength = 0.015,
	},
}
