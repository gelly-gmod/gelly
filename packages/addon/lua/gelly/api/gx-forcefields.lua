gellyx = gellyx or {}
gellyx.forcefield = gellyx.forcefield or {}

---@enum gellyx.forcefield.Mode
gellyx.forcefield.Mode = {
	Force = 0,
	Impulse = 1,
	VelocityChange = 2,
}

---@alias gellyx.forcefield.Params {Position: Vector, Radius: number, Strength: number, LinearFalloff: boolean, Mode: gellyx.forcefield.Mode}

--- Creates a new forcefield. They're implemented as SENTs for semantic reasons, so feel free to use them as such. There should be no burden to properly remove them, as they're automatically removed when the entity is removed.
---@param params gellyx.forcefield.Params
---@return Entity|nil The created forcefield entity
function gellyx.forcefield.create(params)
	local ent = ents.CreateClientside("gellyx_forcefield")
	if not IsValid(ent) then
		return nil
	end

	ent:SetPos(params.Position)
	ent:SetRadius(params.Radius)
	ent:SetStrength(params.Strength)
	ent:SetLinearFalloff(params.LinearFalloff)
	ent:SetMode(params.Mode)
	ent:Spawn()

	return ent
end
