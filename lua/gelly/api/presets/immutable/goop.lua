return {
	Name = "Goop",
	Radius = 5,
	SolverParams = {
		Viscosity = 80,
		SurfaceTension = 0,
		Adhesion = 0.2,
		Cohesion = 1.5,
		VorticityConfinement = 0,
		DynamicFriction = 0.1,
	},
	VisualParams = {
		-- phong shininess
		Shininess = 4000,
		Absorption = Vector(0, 0.01, 0.01),
		RefractionStrength = 0.03,
	},
}
