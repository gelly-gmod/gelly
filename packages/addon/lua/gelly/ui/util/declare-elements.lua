-- Simple Derma utility for creating UI elements declaratively

local function Frame(data)
	return {
		__class = "DFrame",
		__data = data,
	}
end

local function Label(data)
	return {
		__class = "DLabel",
		__data = data,
	}
end

local function Button(data)
	return {
		__class = "DButton",
		__data = data,
	}
end

local function Panel(data)
	return {
		__class = "DPanel",
		__data = data,
	}
end

local function Custom(name, data)
	return {
		__class = name,
		__data = data,
	}
end

local function instantiateElements(panel, elements)
	local endTasks = {}

	for name, element in pairs(elements) do
		local class = element.__class
		local data = element.__data

		print("instantiateElements: Adding " .. class .. " " .. name)
		print(panel)
		local instance = nil
		if name == 1 then
			-- This is the root element
			instance = panel
		else
			instance = panel:Add(class)
			panel[name] = instance
		end

		if data.Children then
			instantiateElements(instance, data.Children)
		end

		for key, value in pairs(data) do
			if key == "Children" then
				-- Do nothing (less evil than using the continue keyword as its not supported in Lua)
			elseif
				key:sub(1, 7) == "DoClick"
				or key == "Paint"
				or key:sub(1, 2) == "On"
			then
				instance[key] = function(...)
					value(panel, ...)
				end
			elseif key == "Center" then
				table.insert(endTasks, function()
					instance:Center()
				end)
			else
				local setFunction = instance["Set" .. key]
				local normalFunction = instance[key]
				if setFunction then
					print("Running: Set" .. key .. " on " .. class)
					if type(value) == "table" then
						-- Nothing in the official API accepts a table as an argument so we can interpret this as multiple arguments
						setFunction(instance, unpack(value))
					else
						setFunction(instance, value)
					end
				elseif normalFunction then
					print("Running: " .. key .. " on " .. class)
					if type(value) == "table" then
						-- Nothing in the official API accepts a table as an argument so we can interpret this as multiple arguments
						normalFunction(instance, unpack(value))
					else
						normalFunction(instance, value)
					end
				else
					print("instantiateElements: No setter for " .. key .. " on " .. class)
				end
			end
		end
	end

	for _, task in ipairs(endTasks) do
		task()
	end
end

return {
	Elements = {
		Frame = Frame,
		Label = Label,
		Button = Button,
		Panel = Panel,
		Custom = Custom,
	},

	instantiateElements = instantiateElements,
}
