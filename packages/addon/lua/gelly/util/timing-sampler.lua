local TimingSampler = {}
TimingSampler.__index = TimingSampler

function TimingSampler.new()
	local self = setmetatable({}, TimingSampler)

	self.renderSamples = {}
	self.simulateSamples = {}
	self.compositeSamples = {}

	self.originalRenderFn = gelly.EndRendering
	self.originalSimulateFn = gelly.EndTick
	self.originalCompositeFn = gelly.Composite

	self.averageRenderMs = 0
	self.averageSimulateMs = 0
	self.averageCompositeMs = 0

	return self
end

function TimingSampler:InjectTimingHooks()
	local self = self

	gelly.EndRendering = function()
		local startTime = SysTime()
		self.originalRenderFn()
		table.insert(self.renderSamples, SysTime() - startTime)
	end

	gelly.EndTick = function(deltaTime)
		local startTime = SysTime()
		self.originalSimulateFn(deltaTime)
		table.insert(self.simulateSamples, SysTime() - startTime)
	end

	gelly.Composite = function()
		local startTime = SysTime()
		self.originalCompositeFn()
		table.insert(self.compositeSamples, SysTime() - startTime)
	end
end

function TimingSampler:RemoveTimingHooks()
	gelly.Render = self.originalRenderFn
	gelly.Simulate = self.originalSimulateFn
	gelly.Composite = self.originalCompositeFn

	self:CalculateAverages()
end

function TimingSampler:CalculateAverages()
	local function calculateAverage(samples)
		local sum = 0
		for _, sample in ipairs(samples) do
			sum = sum + sample
		end

		return sum / #samples
	end

	self.averageRenderMs = calculateAverage(self.renderSamples) * 1000
	self.averageSimulateMs = calculateAverage(self.simulateSamples) * 1000
	self.averageCompositeMs = calculateAverage(self.compositeSamples) * 1000
end

function TimingSampler:GetRenderAverage()
	return self.averageRenderMs
end

function TimingSampler:GetSimulateAverage()
	return self.averageSimulateMs
end

function TimingSampler:GetCompositeAverage()
	return self.averageCompositeMs
end

return TimingSampler
