local array = include("gelly/util/functional-arrays.lua")
local logging = include("gelly/logging.lua")

local function isPropResizerInstalled()
	return scripted_ents.Get("sizehandler") ~= nil
end

if not isPropResizerInstalled() then
	logging.warn("Prop Resizer was not detected. Not loading prop resizer utilities.")
else
	logging.info("Prop Resizer detected. Loading prop resizer utilities.")
end

-- we want to wrap the horrible addon API into a better one so
-- that Gelly can hook into it

local originalAPI = scripted_ents.GetStored("sizehandler").t
local originalThink = originalAPI.Think

function originalAPI:Think()
	originalThink(self)
	local scale = Vector(self:GetActualPhysicsScale())

	if scale ~= self.Gelly_LastScale then
		self.Gelly_LastScale = scale
		hook.Run("GellyPropResized", self:GetParent(), scale)
	end
end

hook.Add("GellyPropResized", "GellyPropResized", function(entity, scale)
	logging.info("Prop resized: %s", tostring(entity))
end)
