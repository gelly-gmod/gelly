--- Setups the given list of network vars for the given entity
--- To get a color picker, use type "Vector" and editType "VectorColor"
---@param vars {name: string, type: string, editType: string?, min: number?, max: number?}[]
return function(ent, vars)
	local perTypeCounters = {}
	for index, var in ipairs(vars) do
		perTypeCounters[var.type] = perTypeCounters[var.type] or 0
		local counter = perTypeCounters[var.type]
		ent:NetworkVar(var.type, counter, var.name,
			{ KeyName = var.name:lower(), Edit = { type = var.editType and var.editType or var.type, order = index + 1, min = var.min, max = var.max } })
		perTypeCounters[var.type] = counter + 1
	end
end
