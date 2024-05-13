return {
	Name = "Blood",
	Radius = 2.50,
	SolverParams = {
		Viscosity = 25,
		SurfaceTension = 0.00,
		Adhesion = 0.3,
		Cohesion = 0.01,
		VorticityConfinement = 0,
		DynamicFriction = 0.1,
	},
	VisualParams = {
		-- phong shininess
		Shininess = 200,
		Absorption = Vector(0.05, 0.5, 0.5),
		RefractionStrength = 0.005,
	},
}
