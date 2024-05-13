return {
	Name = "Goop",
	Radius = 5,
	SolverParams = {
		Viscosity = 20,
		SurfaceTension = 0.0001,
		Adhesion = 0.2,
		Cohesion = 0.2,
		VorticityConfinement = 0,
		DynamicFriction = 0.1,
	},
	VisualParams = {
		-- phong shininess
		Shininess = 4000,
		Absorption = Vector(0, 0.01, 0),
		RefractionStrength = 0.03,
	},
}
