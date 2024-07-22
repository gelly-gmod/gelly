
include("gelly/api/gx-replicate-damage.lua")

hook.Add("OnEntityCreated", "gelly.blood-quantity", function(entity)
	entity.BloodQuantity = entity.BloodQuantity or 100
end)

hook.Add("CreateEntityRagdoll", "gelly.blood-quantity-ragdoll", function(owner, ragdoll)
	ragdoll.BloodQuantity = owner.BloodQuantity
end)