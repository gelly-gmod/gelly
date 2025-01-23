
hook.Add("OnEntityCreated", "gelly.blood-quantity", function(entity)
	if
		not entity:IsValid() or
		(not entity:IsPlayer() and
			not entity:IsNPC() and
			not entity:IsRagdoll() )
	then
		return
	end

	entity.MaxBloodQuantity = entity.MaxBloodQuantity or 100
	entity.BloodQuantity = entity.BloodQuantity or entity.MaxBloodQuantity
end)

hook.Add("CreateEntityRagdoll", "gelly.blood-quantity-ragdoll", function(owner, ragdoll)
	ragdoll.MaxBloodQuantity = owner.MaxBloodQuantity
	ragdoll.BloodQuantity = owner.BloodQuantity
end)