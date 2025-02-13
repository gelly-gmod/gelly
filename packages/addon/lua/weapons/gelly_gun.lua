SWEP.Category = "Gelly"
SWEP.Spawnable = true
SWEP.AdminOnly = false
SWEP.PrintName = "Gelly Gun"

SWEP.ViewModel = "models/weapons/c_pistol.mdl"
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

SWEP.ParticleDensity = 150
SWEP.FireRate = 40      -- bursts per second
SWEP.RapidFireBoost = 6 -- how much proportional quantity of particles to emit when rapid firing

local CROSSHAIR_RADIUS = 10

local INDICATOR_WIDTH_HU = 1
local INDICATOR_HEIGHT_HU = 2
local INDICATOR_HEIGHT_OFFSET = 5

local X_SWAY_PERIOD = 5
local X_SWAY_AMPLITUDE = 0.7
local Y_SWAY_PERIOD = 3
local Y_SWAY_AMPLITUDE = 0.2

local PARTICLE_LIMIT_WARNING_PERCENT = 0.4

local GRABBER_KEY = IN_USE
local FORCEFIELD_SPRITE_TIME = 1
local FORCEFIELD_SPRITE_SIZE = 32

function SWEP:Initialize()
	self:SetHoldType("pistol")
	self:InitializeGrabberHooks()
end

function SWEP:InitializeGrabberHooks()
	if SERVER then
		return
	end

	hook.Add("KeyPress", self, function(_, ply, key)
		if key == GRABBER_KEY and not self:IsInputBlocked() and self:GetOwner():GetActiveWeapon() == self and input.IsButtonDown(MOUSE_MIDDLE) then
			self:OnGrabberKeyPressed()
		end
	end)
end

function SWEP:GetForcefieldDistance()
	return math.min(self:GetOwner():GetEyeTrace().HitPos:Distance(self:GetOwner():GetShootPos()), self.InitialDistance)
end

function SWEP:OnGrabberKeyPressed()
	if self.Forcefield then
		surface.PlaySound("buttons/button10.wav")
		self.Forcefield:Remove()
		self.Forcefield = nil
	else
		surface.PlaySound("buttons/button9.wav")
		self.Forcefield = gellyx.forcefield.create({
			Position = self:GetOwner():GetShootPos(),
			Radius = gellyx.settings.get("gelly_gun_forcefield_radius"):GetFloat(),
			Strength = gellyx.settings.get("gelly_gun_forcefield_strength"):GetFloat(),
			LinearFalloff = false,
			Mode = gellyx.forcefield.Mode.Force,
		})

		self.InitialDistance = self:GetOwner():GetEyeTrace().HitPos:Distance(self:GetOwner():GetShootPos())
		self.ForcefieldActivationTime = CurTime()
	end
end

function SWEP:OnGrabberThink()
	if not self.Forcefield then
		return
	end

	if self.LastForcefieldPosition then
		self.Forcefield:SetPos(LerpVector(0.02, self.Forcefield:GetPos(), self.LastForcefieldPosition))
	end

	local owner = self:GetOwner()
	local forcefieldPosition = owner:GetShootPos() + owner:GetAimVector() * self:GetForcefieldDistance()
	if self.LastForcefieldPosition then
		self.Forcefield:SetPos(LerpVector(0.02, self.Forcefield:GetPos(), self.LastForcefieldPosition))
	else
		self.Forcefield:SetPos(forcefieldPosition)
	end
	self.LastForcefieldPosition = forcefieldPosition
end

function SWEP:GetPrimaryBounds()
	local size = gellyx.settings.get("gelly_gun_primary_size"):GetFloat()
	return Vector(size, size, size)
end

function SWEP:GetSecondaryBounds()
	local size = gellyx.settings.get("gelly_gun_secondary_size"):GetFloat()
	return Vector(size, size, size)
end

function SWEP:PrimaryAttack()
	if SERVER then
		self:CallOnClient("PrimaryAttack")
		return
	end
	---@type Player
	local owner = self:GetOwner()

	gellyx.emitters.Cube({
		center = owner:GetShootPos() + owner:GetAimVector() * gellyx.settings.get("gelly_gun_distance"):GetFloat(),
		velocity = owner:GetAimVector() * 2,
		bounds = self:GetPrimaryBounds(),
		density = gellyx.settings.get("gelly_gun_density"):GetInt() / gellyx.presets.getEffectiveRadius(),
		invMass = 1 / gellyx.settings.get("particle_mass"):GetFloat(),
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
		center = owner:GetShootPos() + owner:GetAimVector() * gellyx.settings.get("gelly_gun_distance"):GetFloat(),
		velocity = owner:GetAimVector() * gellyx.settings.get("gelly_gun_secondary_velocity"):GetFloat(),
		bounds = self:GetSecondaryBounds(),
		density = gellyx.settings.get("gelly_gun_density"):GetInt() / gellyx.presets.getEffectiveRadius(),
		invMass = 1 / gellyx.settings.get("particle_mass"):GetFloat(),
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

function SWEP:IsInputBlocked()
	return vgui.GetKeyboardFocus() or gui.IsConsoleVisible() or gui.IsGameUIVisible()
end

local forcefieldSprite
if CLIENT then
	forcefieldSprite = Material("sprites/light_glow02_add")
end

function SWEP:ViewModelDrawn(vm)
	-- draw some 3D status indicators on the viewmodel
	-- top left corner
	local muzzleOrigin = vm:GetAttachment(vm:LookupAttachment("muzzle")).Pos
	if self.Forcefield then
		render.SetMaterial(forcefieldSprite)
		local deltaTime = math.min(CurTime() - self.ForcefieldActivationTime, FORCEFIELD_SPRITE_TIME) /
			FORCEFIELD_SPRITE_TIME
		-- sine ease out: https://easings.net/#easeOutSine
		deltaTime = math.sin((deltaTime * math.pi) / 2)

		local spriteSize = FORCEFIELD_SPRITE_SIZE * deltaTime
		render.DrawSprite(muzzleOrigin + self.Owner:GetAimVector() * 30, spriteSize, spriteSize, Color(100, 100, 255))
	end

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
		"Press R for the menu or E to adjust time scale",
		"DermaLarge",
		0,
		25,
		Color(255, 255, 255),
		TEXT_ALIGN_LEFT,
		TEXT_ALIGN_CENTER
	)

	if input.IsKeyDown(KEY_R) and not self:IsInputBlocked() then
		self:CreateMenu()
	end

	if input.IsKeyDown(KEY_E) and not self:IsInputBlocked() and not input.IsButtonDown(MOUSE_MIDDLE) then
		self:CreateTimeSlider()
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

function SWEP:Think()
	self:OnGrabberThink()
end

function SWEP:CreateMenu()
	if GELLY_GUN_MENU then
		return
	end

	GELLY_GUN_MENU = vgui.Create("GellyGunMenu")
end

function SWEP:CreateTimeSlider()
	if GELLY_GUN_TIME_SLIDER then
		return
	end

	GELLY_GUN_TIME_SLIDER = vgui.Create("GellyGunTimeSlider")
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

	PANEL.ArcSegments = 32
	PANEL.ArcAngleBias = 0 -- This makes it so menu options start out at left and right instead of top and bottom
	PANEL.LastSelectedOption = 1
	PANEL.LastSelectedOptionTime = CurTime()
	PANEL.NeutralZoneRadius = 120 -- neutral zone cancels selection

	function PANEL:Init()
		self:SetSize(ScrW(), ScrH())
		self:Center()
		self:MakePopup()
		self:SetTitle("Gelly Gun Menu")
		self:ShowCloseButton(false)
		self:SetDraggable(false)
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
				surface.PlaySound("garrysmod/ui_click.wav")
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

		local normalizedMouseX = gui.MouseX() / ScrW()
		local normalizedMouseY = gui.MouseY() / ScrH()

		local userCursorAngle =
			math.atan2(gui.MouseY() / ScrH() - 0.5, gui.MouseX() / ScrW() - 0.5)

		-- we just take the sum of the squares of the normalized mouse vector to the center to get the distance
		local isCursorInNeutralZone = math.sqrt(
			(normalizedMouseX - 0.5) ^ 2 + (normalizedMouseY - 0.5) ^ 2
		) < self.NeutralZoneRadius / ScrW()

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

			local points = {}
			-- in order to have a circle in the middle, we need to render every outer arc point connected to their neighbors,
			-- then a reversed version of the inner arc points, then the outer arc points again in reverse order so that
			-- the GMod triangulation doesn't mess up the inner arc
			for arcSegment = 0, self.ArcSegments do
				local arcX = centerX
					+ math.cos(
						angle + (arcAnglePadding / self.ArcSegments) * arcSegment
					)
					* self.NeutralZoneRadius
				local arcY = centerY
					+ math.sin(
						angle + (arcAnglePadding / self.ArcSegments) * arcSegment
					)
					* self.NeutralZoneRadius

				-- start the actual arc from the neutral zone
				local extendedArcX = arcX
					+ math.cos(
						angle + (arcAnglePadding / self.ArcSegments) * arcSegment
					)
					* (radius - self.NeutralZoneRadius)

				local extendedArcY = arcY
					+ math.sin(
						angle + (arcAnglePadding / self.ArcSegments) * arcSegment
					)
					* (radius - self.NeutralZoneRadius)

				table.insert(points, { x = extendedArcX, y = extendedArcY })
			end

			-- revert the points so we can draw the inner arc
			for arcSegment = self.ArcSegments, 0, -1 do
				local arcX = centerX
					+ math.cos(
						angle + (arcAnglePadding / self.ArcSegments) * arcSegment
					)
					* self.NeutralZoneRadius
				local arcY = centerY
					+ math.sin(
						angle + (arcAnglePadding / self.ArcSegments) * arcSegment
					)
					* self.NeutralZoneRadius

				table.insert(points, { x = arcX, y = arcY })
			end

			-- and revert the points again so we can draw the outer arc
			for arcSegment = self.ArcSegments, 0, -1 do
				local arcX = centerX
					+ math.cos(
						angle + (arcAnglePadding / self.ArcSegments) * arcSegment
					)
					* self.NeutralZoneRadius
				local arcY = centerY
					+ math.sin(
						angle + (arcAnglePadding / self.ArcSegments) * arcSegment
					)
					* self.NeutralZoneRadius

				local extendedArcX = arcX
					+ math.cos(
						angle + (arcAnglePadding / self.ArcSegments) * arcSegment
					)
					* (radius - self.NeutralZoneRadius)

				local extendedArcY = arcY
					+ math.sin(
						angle + (arcAnglePadding / self.ArcSegments) * arcSegment
					)
					* (radius - self.NeutralZoneRadius)

				table.insert(points, { x = extendedArcX, y = extendedArcY })
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
				and not isCursorInNeutralZone

			if isSelected then
				self.ActiveOption = option

				if self.LastSelectedOption ~= i then
					self.LastSelectedOption = i
					self.LastSelectedOptionTime = CurTime()
					surface.PlaySound("buttons/lightswitch2.wav")
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
				+ math.cos(angle + arcAnglePadding / 2) * self.NeutralZoneRadius * 1.5
			local arcCenterY = centerY
				+ math.sin(angle + arcAnglePadding / 2) * self.NeutralZoneRadius * 1.5

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

local function createTimeSliderPanel()
	local PANEL = {}
	PANEL.SliderWidthOfScreen = 0.8
	PANEL.SliderHeightOfScreen = 0.02
	PANEL.BlurIterations = 4
	PANEL.Min = 0.0001
	PANEL.Max = 20
	PANEL.Value = 1

	function PANEL:Init()
		self:SetSize(ScrW(), ScrH())
		self:Center()
		self:MakePopup()
		self:SetTitle("Gelly Gun Time Slider")
		self:ShowCloseButton(false)
		self:SetDraggable(false)
	end

	function PANEL:Paint(w, h)
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

		if not input.IsKeyDown(KEY_E) then
			GELLY_SIM_TIMESCALE = self.Value
			self:Remove()
			GELLY_GUN_TIME_SLIDER = nil
			return
		end

		-- darken the background
		surface.SetDrawColor(0, 0, 0, 200)
		surface.DrawRect(0, 0, w, h)

		-- draw the slider
		local sliderWidth = w * self.SliderWidthOfScreen
		local sliderHeight = h * self.SliderHeightOfScreen

		local sliderX = w / 2 - sliderWidth / 2
		local sliderY = h / 2 - sliderHeight / 2

		draw.RoundedBox(8, sliderX, sliderY, sliderWidth, sliderHeight, Color(50, 50, 50))

		-- draw helpful arrows
		local slowerArrowX = sliderX + 120
		local slowerArrowY = sliderY - sliderHeight * 2

		local fasterArrowX = sliderX + sliderWidth - 120
		local fasterArrowY = sliderY - sliderHeight * 2

		draw.SimpleText(
			"← Slower",
			"DermaLarge",
			slowerArrowX,
			slowerArrowY,
			Color(255, 255, 255),
			TEXT_ALIGN_CENTER,
			TEXT_ALIGN_CENTER
		)

		draw.SimpleText(
			"Faster →",
			"DermaLarge",
			fasterArrowX,
			fasterArrowY,
			Color(255, 255, 255),
			TEXT_ALIGN_CENTER,
			TEXT_ALIGN_CENTER
		)

		-- draw min + max labels
		surface.SetDrawColor(255, 255, 255)
		local minX = sliderX
		local minY = sliderY + sliderHeight + 40
		surface.DrawLine(sliderX, sliderY + sliderHeight + 10, minX, minY)

		local maxX = sliderX + sliderWidth
		local maxY = sliderY + sliderHeight + 40
		surface.DrawLine(sliderX + sliderWidth, sliderY + sliderHeight + 10, maxX, maxY)

		local minText = ("%d%%"):format(self.Min * 100)
		local maxText = ("%d%%"):format(self.Max * 100)

		local _, minTextSizeY = surface.GetTextSize(minText)
		local _, maxTextSizeY = surface.GetTextSize(maxText)

		draw.SimpleText(
			minText,
			"DermaLarge",
			minX,
			minY + minTextSizeY,
			Color(255, 255, 255),
			TEXT_ALIGN_CENTER,
			TEXT_ALIGN_CENTER
		)

		draw.SimpleText(
			maxText,
			"DermaLarge",
			maxX,
			maxY + maxTextSizeY,
			Color(255, 255, 255),
			TEXT_ALIGN_CENTER,
			TEXT_ALIGN_CENTER
		)

		local sliderProgress = (self.Value - self.Min) / (self.Max - self.Min)

		draw.RoundedBox(
			8,
			sliderX,
			sliderY,
			sliderWidth * sliderProgress,
			sliderHeight,
			Color(100, 100, 255)
		)

		-- Update the slider by mouse position

		local mouseX = math.max(sliderX, math.min(sliderX + sliderWidth, gui.MouseX()))

		self.Value =
			math.Remap(mouseX, sliderX, sliderX + sliderWidth, self.Min, self.Max)

		-- draw the value
		local valueText = ("%d%%"):format(self.Value * 100)
		local _, valueTextSizeY = surface.GetTextSize(valueText)

		local valueTextX = mouseX
		local valueTextY = sliderY - valueTextSizeY - 10

		surface.DrawLine(mouseX, sliderY + sliderHeight, mouseX, valueTextY)

		draw.SimpleText(
			valueText,
			"DermaLarge",
			valueTextX,
			valueTextY,
			Color(255, 255, 255),
			TEXT_ALIGN_CENTER,
			TEXT_ALIGN_CENTER
		)

		-- Update the time scale, for instant feedback
		GELLY_SIM_TIMESCALE = self.Value
	end

	vgui.Register("GellyGunTimeSlider", PANEL, "DFrame")
end

-- autorefresh support
if gellyx and CLIENT then
	createMenuPanel()
	createTimeSliderPanel()
end

hook.Add("GellyLoaded", "gellygun.make-menu-panel", function()
	createMenuPanel()
	createTimeSliderPanel()
end)
