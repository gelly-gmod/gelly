---@alias gelly.functional.Predicate fun(value: any, index: number, array: FunctionalArray): boolean
---@alias gelly.functional.Mapper fun(value: any, index: number, array: FunctionalArray): any
---@alias gelly.functional.Reducer fun(accumulator: any, value: any, index: number, array: FunctionalArray): any
---@alias gelly.functional.Action fun(value: any, index: number, array: FunctionalArray)

---@class FunctionalArray
---@field private array table
--- A wrapper around an array that provides functional programming methods.

local FunctionalArray = {}
FunctionalArray.__index = FunctionalArray

function FunctionalArray.new(array)
	local self = setmetatable({}, FunctionalArray)
	self.array = array
	return self
end

function FunctionalArray.__tostring(self)
	return ("[%s]"):format(table.concat(self.array, ", "))
end

--- Returns a new FunctionalArray with the elements that pass the predicate.
---@param predicate gelly.functional.Predicate
---@return FunctionalArray
function FunctionalArray:filter(predicate)
	local newArray = {}

	for index, value in ipairs(self.array) do
		if predicate(value, index, self) then
			table.insert(newArray, value)
		end
	end

	return FunctionalArray.new(newArray)
end

--- Returns a new FunctionalArray with the results of applying the mapper to each element.
---@param mapper gelly.functional.Mapper
---@return FunctionalArray
function FunctionalArray:map(mapper)
	local newArray = {}

	for index, value in ipairs(self.array) do
		newArray[index] = mapper(value, index, self)
	end

	return FunctionalArray.new(newArray)
end

--- Reduces the FunctionalArray to a single value.
---@param reducer gelly.functional.Reducer
---@param initialValue any
---@return any
function FunctionalArray:reduce(reducer, initialValue)
	local accumulator = initialValue

	for index, value in ipairs(self.array) do
		accumulator = reducer(accumulator, value, index, self)
	end

	return accumulator
end

--- Returns the first element that passes the predicate.
---@param predicate gelly.functional.Predicate
---@return any
function FunctionalArray:find(predicate)
	for index, value in ipairs(self.array) do
		if predicate(value, index, self) then
			return value
		end
	end

	return nil
end

--- Returns if any element passes the predicate.
---@param predicate gelly.functional.Predicate
---@return boolean
function FunctionalArray:any(predicate)
	for index, value in ipairs(self.array) do
		if predicate(value, index, self) then
			return true
		end
	end

	return false
end

--- Returns if all elements pass the predicate.
---@param predicate gelly.functional.Predicate
---@return boolean
function FunctionalArray:all(predicate)
	for index, value in ipairs(self.array) do
		if not predicate(value, index, self) then
			return false
		end
	end

	return true
end

--- Simply iterates over the array.
---@param action gelly.functional.Action
function FunctionalArray:forEach(action)
	for index, value in ipairs(self.array) do
		action(value, index, self)
	end
end

--- Decomposes into a standard Lua table.
---@return table
function FunctionalArray:toArray()
	return self.array
end

--- Constructs a new FunctionalArray from the elements of the current array.
return function(array)
	return FunctionalArray.new(array)
end
