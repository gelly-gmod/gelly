return function(options)
	local panel = options.Panel
	local amount = options.Amount or 5
	local darkenAlpha = options.DarkenAlpha or 200

	-- draw a blurred background
	local x, y = panel:LocalToScreen(0, 0)

	surface.SetDrawColor(255, 255, 255)
	surface.SetMaterial(Material("pp/blurscreen"))
	for i = 1, amount do
		Material("pp/blurscreen"):SetFloat("$blur", (i / amount) * 6)
		Material("pp/blurscreen"):Recompute()

		render.UpdateScreenEffectTexture()
		render.SetScissorRect(x, y, x + options.W, y + options.H, true)
		surface.DrawTexturedRect(0, 0, ScrW(), ScrH())
		render.SetScissorRect(0, 0, 0, 0, false)
	end

	-- darken the background
	surface.SetDrawColor(0, 0, 0, darkenAlpha)
	render.SetScissorRect(x, y, x + options.W, y + options.H, true)
	surface.DrawRect(0, 0, ScrW(), ScrH())
	render.SetScissorRect(0, 0, 0, 0, false)
end
