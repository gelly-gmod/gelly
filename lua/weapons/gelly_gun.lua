SWEP.Category = "Gelly"
SWEP.Spawnable = true
SWEP.AdminOnly = false
SWEP.PrintName = "Gelly Gun"

SWEP.ViewModel = "models/weapons/v_pistol.mdl"
SWEP.WorldModel = "models/weapons/w_pistol.mdl"
SWEP.ViewModelFOV = 54
SWEP.UseHands = true

SWEP.Primary.ClipSize = -1
SWEP.Primary.DefaultClip = -1
SWEP.Primary.Automatic = true
SWEP.Primary.Ammo = ""

SWEP.Secondary.ClipSize = -1
SWEP.Secondary.DefaultClip = -1
SWEP.Secondary.Automatic = true
SWEP.Secondary.Ammo = ""

SWEP.ParticleDensity = 300
SWEP.FireRate = 40 -- bursts per second
SWEP.RapidFireBoost = 2 -- how much proportional quantity of particles to emit when rapid firing

local CROSSHAIR_COLOR = Color(100, 100, 255)
local CROSSHAIR_RADIUS = 10

local INDICATOR_WIDTH_HU = 1
local INDICATOR_HEIGHT_HU = 2
local INDICATOR_HEIGHT_OFFSET = 5

local X_SWAY_PERIOD = 5
local X_SWAY_AMPLITUDE = 0.7
local Y_SWAY_PERIOD = 3
local Y_SWAY_AMPLITUDE = 0.2

local PARTICLE_LIMIT_WARNING_PERCENT = 0.4

function SWEP:Initialize()
	self:SetHoldType("pistol")
end

function SWEP:PrimaryAttack()
	if SERVER then
		self:CallOnClient("PrimaryAttack")
		return
	end

	---@type Player
	local owner = self:GetOwner()

	gellyx.emitters.Cube({
		center = owner:GetShootPos() + owner:GetAimVector() * 110,
		velocity = owner:GetAimVector() * 2,
		bounds = Vector(50, 50, 50),
		density = self.ParticleDensity,
	})

	self:SetNextPrimaryFire(CurTime() + 1 / self.FireRate)
end

function SWEP:SecondaryAttack()
	if SERVER then
		self:CallOnClient("SecondaryAttack")
		return
	end

	local owner = self:GetOwner()

	gellyx.emitters.Cube({
		center = owner:GetShootPos() + owner:GetAimVector() * 110,
		velocity = owner:GetAimVector() * 70,
		bounds = Vector(10, 10, 10),
		density = self.ParticleDensity * self.RapidFireBoost,
	})

	self:SetNextSecondaryFire(CurTime() + 1 / self.FireRate * self.RapidFireBoost)
end

function SWEP:CreateCrosshairColorFromActivePreset()
	-- try to generate a color from a fluid's extinction coefficient
	local activePreset = gellyx.presets.getActivePreset()
	local absorption = activePreset.Material.Absorption

	local default_t = 75 -- pretending we're looking through 75 hammer units of fluid
	local r = math.exp(-absorption.x * default_t)
	local g = math.exp(-absorption.y * default_t)
	local b = math.exp(-absorption.z * default_t)

	local colorBoostFactor = 1.2 -- arbitrary factor to boost the color's vibrance
	r = math.min(1, r * colorBoostFactor)
	g = math.min(1, g * colorBoostFactor)
	b = math.min(1, b * colorBoostFactor)

	return Color(r * 255, g * 255, b * 255)
end

function SWEP:DoDrawCrosshair(x, y)
	surface.DrawCircle(
		x,
		y,
		CROSSHAIR_RADIUS,
		self:CreateCrosshairColorFromActivePreset()
	)
end

function SWEP:GetViewModelSwayVector()
	-- B = 2pi / P
	local swayX = math.sin(CurTime() * 2 * math.pi / X_SWAY_PERIOD) * X_SWAY_AMPLITUDE
	local swayY = math.sin(CurTime() * 2 * math.pi / Y_SWAY_PERIOD) * Y_SWAY_AMPLITUDE

	return Vector(swayX, 0, swayY)
end

function SWEP:IsParticleLimitNear()
	return gelly.GetStatus().ActiveParticles
		>= gelly.GetStatus().MaxParticles * PARTICLE_LIMIT_WARNING_PERCENT
end

function SWEP:ViewModelDrawn(vm)
	-- draw some 3D status indicators on the viewmodel

	-- top left corner
	local muzzleOrigin = vm:GetAttachment(vm:LookupAttachment("muzzle")).Pos
	-- convert that to local space
	muzzleOrigin = vm:WorldToLocal(muzzleOrigin)

	local indicatorOrigin = vm:GetWorldTransformMatrix()
		* (
			Vector(40, INDICATOR_WIDTH_HU, INDICATOR_HEIGHT_OFFSET + INDICATOR_HEIGHT_HU)
			+ muzzleOrigin
			+ self:GetViewModelSwayVector()
		)

	local viewModelLeft = -vm:GetRight()
	local viewModelUp = vm:GetUp()

	local indicatorAngle = viewModelLeft:AngleEx(viewModelLeft:Cross(-viewModelUp))
	-- flip pitch
	indicatorAngle:RotateAroundAxis(viewModelLeft, 180)
	-- then turn it to face the player
	indicatorAngle:RotateAroundAxis(viewModelUp, 180)

	cam.Start3D2D(indicatorOrigin, indicatorAngle, 0.05) -- 1:20 scale
	draw.SimpleText(
		("%s"):format(gellyx.presets.getActivePreset().Name),
		"DermaLarge",
		0,
		0,
		self:CreateCrosshairColorFromActivePreset(),
		TEXT_ALIGN_LEFT,
		TEXT_ALIGN_CENTER
	)

	draw.SimpleText(
		"Press R for the menu",
		"DermaLarge",
		0,
		25,
		Color(255, 255, 255),
		TEXT_ALIGN_LEFT,
		TEXT_ALIGN_CENTER
	)

	if input.IsKeyDown(KEY_R) then
		self:CreateMenu()
	end

	if self:IsParticleLimitNear() then
		-- show the particle progress bar
		local progress = gelly.GetStatus().ActiveParticles
			/ gelly.GetStatus().MaxParticles

		local warningStartColor = Color(230, 100, 0)
		local warningEndColor = Color(255, 0, 0)
		local colorProgress =
			math.Remap(progress, PARTICLE_LIMIT_WARNING_PERCENT, 1, 0, 1)

		local color = Color(
			Lerp(colorProgress, warningStartColor.r, warningEndColor.r),
			Lerp(colorProgress, warningStartColor.g, warningEndColor.g),
			Lerp(colorProgress, warningStartColor.b, warningEndColor.b),
			200
		)

		draw.RoundedBox(8, 0, 50, 200, 10, Color(0, 0, 0, 200))
		draw.RoundedBox(8, 0, 50, 200 * progress, 10, color)
	end

	cam.End3D2D()
end

function SWEP:CreateMenu()
	if GELLY_GUN_MENU then
		return
	end

	GELLY_GUN_MENU = vgui.Create("GellyGunMenu")
end

local function createMenuPanel()
	local PANEL = {}
	PANEL.BlurIterations = 4
	PANEL.MenuOptions = {}
	for _, preset in pairs(gellyx.presets.getAllPresets()) do
		table.insert(PANEL.MenuOptions, {
			Name = preset.Name,
			OnSelect = function()
				gelly.Reset()
				gellyx.presets.select(preset.Name)
			end,
		})
	end

	table.insert(PANEL.MenuOptions, {
		Name = "Clear particles",
		OnSelect = function()
			gelly.Reset()
		end,
	})

	table.insert(PANEL.MenuOptions, {
		Name = "Close menu",
		OnSelect = function() end,
	})

	PANEL.ArcSegments = 16
	PANEL.ArcAngleBias = 0 -- This makes it so menu options start out at left and right instead of top and bottom
	PANEL.LastSelectedOption = 1
	PANEL.LastSelectedOptionTime = CurTime()

	function PANEL:Init()
		self:SetSize(ScrW(), ScrH())
		self:Center()
		self:MakePopup()
		self:SetTitle("Gelly Gun Menu")
		self:ShowCloseButton(true)
		self:SetDraggable(true)
	end

	function PANEL:CalculateFadeIn()
		local fadeInTime = 0.1
		local fadeInProgress =
			math.min((CurTime() - self.LastSelectedOptionTime) / fadeInTime, 1)

		return fadeInProgress
	end

	function PANEL:Paint(w, h)
		if not input.IsKeyDown(KEY_R) then
			if self.ActiveOption then
				self.ActiveOption.OnSelect()
			end

			self:Remove()
			GELLY_GUN_MENU = nil
			return
		end

		-- draw a blurred background
		local x, y = self:LocalToScreen(0, 0)

		surface.SetDrawColor(255, 255, 255)
		surface.SetMaterial(Material("pp/blurscreen"))
		for i = 1, self.BlurIterations do
			Material("pp/blurscreen"):SetFloat("$blur", (i / self.BlurIterations) * 6)
			Material("pp/blurscreen"):Recompute()
			render.UpdateScreenEffectTexture()
			render.SetScissorRect(x, y, x + w, y + h, true)
			surface.DrawTexturedRect(0, 0, ScrW(), ScrH())
			render.SetScissorRect(0, 0, 0, 0, false)
		end

		-- darken the background
		surface.SetDrawColor(0, 0, 0, 200)
		surface.DrawRect(0, 0, w, h)

		local userCursorAngle =
			math.atan2(gui.MouseY() / ScrH() - 0.5, gui.MouseX() / ScrW() - 0.5)

		if userCursorAngle < 0 then
			userCursorAngle = userCursorAngle + 2 * math.pi
		end

		-- draw a radial menu
		for i, option in ipairs(self.MenuOptions) do
			local angle = math.rad(360 / #self.MenuOptions * i) + self.ArcAngleBias
			local radius = ScrH() / 3
			local arcAnglePadding = math.rad(360 / #self.MenuOptions)

			local centerX = w / 2
			local centerY = h / 2

			local points = {
				{ x = centerX, y = centerY },
			}

			-- discretize the arc into segments
			for arcSegment = 0, self.ArcSegments do
				local arcX = centerX
					+ math.cos(
							angle + (arcAnglePadding / self.ArcSegments) * arcSegment
						)
						* radius
				local arcY = centerY
					+ math.sin(
							angle + (arcAnglePadding / self.ArcSegments) * arcSegment
						)
						* radius

				table.insert(points, { x = arcX, y = arcY })
			end

			local wrappedAngle = angle
			local wrappedAngleWithPadding = angle + arcAnglePadding

			-- there's an edge case where the angle wraps around 0

			if wrappedAngle >= 2 * math.pi then
				wrappedAngle = wrappedAngle - 2 * math.pi
			end

			if wrappedAngleWithPadding >= 2 * math.pi then
				wrappedAngleWithPadding = wrappedAngleWithPadding - 2 * math.pi
			end

			if
				angle + arcAnglePadding >= 2 * math.pi
				and wrappedAngle < 2 * math.pi
				and wrappedAngle > 0
			then
				wrappedAngleWithPadding = 2 * math.pi
			end

			local isSelected = userCursorAngle > wrappedAngle
				and userCursorAngle < wrappedAngleWithPadding

			if isSelected then
				self.ActiveOption = option

				if self.LastSelectedOption ~= i then
					self.LastSelectedOption = i
					self.LastSelectedOptionTime = CurTime()
				end
			end

			draw.NoTexture()
			local fade = isSelected and self:CalculateFadeIn() or 1
			surface.SetDrawColor(
				isSelected and Color(50 + 30 * fade, 50 + 30 * fade, 50 + 30 * fade)
					or Color(50, 50, 50)
			)

			surface.DrawPoly(points)

			local arcCenterX = centerX
				+ math.cos(angle + arcAnglePadding / 2) * radius / 2
			local arcCenterY = centerY
				+ math.sin(angle + arcAnglePadding / 2) * radius / 2

			draw.SimpleText(
				option.Name,
				"ChatFont",
				arcCenterX,
				arcCenterY,
				Color(255, 255, 255),
				TEXT_ALIGN_CENTER,
				TEXT_ALIGN_CENTER
			)
		end
	end

	vgui.Register("GellyGunMenu", PANEL, "DFrame")
end

-- autorefresh support
if gellyx and CLIENT then
	createMenuPanel()
end

hook.Add("GellyLoaded", "gellygun.make-menu-panel", createMenuPanel)
