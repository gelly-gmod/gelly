gellyx = gellyx or {}
gellyx.mods = gellyx.mods or {}

---@enum gellyx.mods.ModType
--- Enum for mod types.
gellyx.mods.ModType = {
	--- A mod that acts in a global context. (e.g: a mod that rains liquid or a lava game mode)
	Global = 1,
	--- A mod that acts in a local context, invariant of any current fluid. (e.g: a mod that lets people grab chunks of fluid)
	Local = 2,
}
