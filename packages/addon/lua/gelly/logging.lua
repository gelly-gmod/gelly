-- Debug logging driven by a convar

local logEnabledConVar = CreateConVar(
	"gelly_log_enabled",
	"0",
	FCVAR_ARCHIVE,
	"Enable debug logging for gelly-gmod"
)

local function isLogEnabled()
	return logEnabledConVar:GetBool()
end

local function info(message, ...)
	if isLogEnabled() then
		print(("[gelly-gmod == LOG @ %s] %s"):format(debug.getinfo(2, "n").name, (message):format(...)))
	end
end

local function warn(message, ...)
	if isLogEnabled() then
		print("[gelly-gmod == WARN] " .. (message):format(...))
	end
end

local function logError(message, ...)
	if isLogEnabled() then
		print("[gelly-gmod == ERROR] " .. (message):format(...))
		error(message)
	end
end

return {
	info = info,
	warn = warn,
	error = logError,
}
