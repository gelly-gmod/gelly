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
		print("[gelly-gmod == INFO] " .. (message):format(...))
	end
end

local function warn(message, ...)
	if isLogEnabled() then
		print("[gelly-gmod == WARN] " .. (message):format(...))
	end
end

local function error(message, ...)
	if isLogEnabled() then
		print("[gelly-gmod == ERROR] " .. (message):format(...))
		error(message)
	end
end

return {
	info = info,
	warn = warn,
	error = error,
}
