local SPAWNMENU_SCHEMA = {
	["Fun Stuff"] = {
		icon = "icon16/rainbow.png",
		children = {
			{
				nicename = "Disc Emitter",
				spawnname = "gelly_disc_emitter",
				weapon = false,
				admin = false,
			},
			{
				nicename = "Forcefield Ball",
				spawnname = "gelly_forcefield",
				weapon = false,
				admin = false,
			}
		}
	},
	["Guns"] = {
		icon = "icon16/gun.png",
		children = {
			{
				nicename = "Gelly Gun",
				spawnname = "gelly_gun",
				weapon = true,
				admin = false,
			},
			{
				nicename = "Gelly Liquifier",
				spawnname = "gelly_liquifier",
				weapon = true,
				admin = false,
			}
		}
	}
}

hook.Add("PopulateGellyItems", "gelly.add-spawnmenu-items", function(pnlContent, tree, browseNode)
	for category, info in pairs(SPAWNMENU_SCHEMA) do
		local node = tree:AddNode(category, info.icon or "icon16/bricks.png")
		node.DoPopulate = function(self)
			if self.PropPanel then return end

			self.PropPanel = vgui.Create("ContentContainer", pnlContent)
			self.PropPanel:SetVisible(false)
			self.PropPanel:SetTriggerSpawnlistChange(false)

			for _, item in ipairs(info.children) do
				spawnmenu.CreateContentIcon(item.weapon and "weapon" or "entity", self.PropPanel, {
					nicename = item.nicename,
					material = item.material or "entities/" .. item.spawnname .. ".png",
					admin = item.admin,
					weapon = item.weapon,
					spawnname = item.spawnname
				})
			end
		end

		if browseNode then
			browseNode:AddNode(node)
		end

		node.DoClick = function(self)
			self:DoPopulate()
			pnlContent:SwitchPanel(self.PropPanel)
		end
	end
end)

local function createGellyTab()
	local containerPanel = vgui.Create("SpawnmenuContentPanel")
	containerPanel:EnableSearch("weapon", "PopulateGellyItems")
	containerPanel:CallPopulateHook("PopulateGellyItems")

	return containerPanel
end

spawnmenu.AddCreationTab("Gelly", createGellyTab, "icon16/gelly.png", 10, "All things Gelly")
