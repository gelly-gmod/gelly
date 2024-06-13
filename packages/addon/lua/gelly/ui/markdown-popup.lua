local PANEL = {}

AccessorFunc(PANEL, "OnAction", "OnAction")
AccessorFunc(PANEL, "PopupTitle", "PopupTitle")
AccessorFunc(PANEL, "Markdown", "Markdown")
AccessorFunc(PANEL, "Action", "Action")

function PANEL:Popup()
	self.HTML = self:Add("DHTML")
	self.HTML:Dock(FILL)
	self.HTML:OpenURL("asset://garrysmod/lua/html/markdown_popup.html.lua")
	self:SetVisible(false)
	self:SetSize(ScrW() * 0.5, ScrH() * 0.5)
	self:Center()
	self:SetupJSEnvironment()

	self.HTML.OnDocumentReady = function()
		self:SetVisible(true)
		self:MakePopup()
	end
end

function PANEL:Paint(w, h)
	local x, y = self:LocalToScreen(0, 0)

	render.SetScissorRect(x, y, x + w, y + h, true)
	Derma_DrawBackgroundBlur(self, 0);
	render.SetScissorRect(0, 0, 0, 0, false)
end

function PANEL:SetupJSEnvironment()
	self.HTML:AddFunction("popup", "performAction", function()
		self.OnAction()
	end)

	self.HTML:AddFunction("popup", "close", function()
		self:Remove()
	end)

	self.HTML:AddFunction("popup", "getMarkdown", function()
		print("Sending markdown: ", self:GetMarkdown())
		return self:GetMarkdown()
	end)

	self.HTML:AddFunction("popup", "getAction", function()
		return self:GetAction()
	end)

	self.HTML:AddFunction("popup", "getTitle", function()
		return self:GetPopupTitle()
	end)
end

vgui.Register("GellyNewVersionPopup", PANEL, "EditablePanel")
