--- Tries converting a color to an absorption value, although
--- this doesn't always produce accurate results.
return function(color)
	local rNormalized = color.r / 255
	local gNormalized = color.g / 255
	local bNormalized = color.b / 255

	-- We use the formula 1 - e^(-x) to convert the normalized color
	local rAbsorption = 1 - rNormalized
	local gAbsorption = 1 - gNormalized
	local bAbsorption = 1 - bNormalized

	return Vector(rAbsorption, gAbsorption, bAbsorption)
end
