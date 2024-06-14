include("gelly/ui/new-version-popup.lua")

local GH_RELEASES_API_URL = "https://api.github.com/repos/yogwoggf/gelly/releases/latest"
local FAVORED_RELEASE_ASSET = "gelly-gmod-release-x64.zip" -- we favor the release build than the debug build

---@alias GellyParsedRelease {version: string, releaseNotes: string, downloadURL: string}

--- Parses the release API response given by GitHub
---@param responseJSON string The JSON response from the GitHub API
---@return GellyParsedRelease|nil release The parsed release information. Nil if the release is not found, which may happen if someone has yanked the release
local function parseReleaseResponse(responseJSON)
	local response = util.JSONToTable(responseJSON)
	if not response then
		error("Failed to parse JSON response from GitHub API")
	end

	local release = {
		version = response.tag_name,
		releaseNotes = response.body,
		downloadURL = nil
	}

	release.releaseNotes = string.gsub(release.releaseNotes, "\r\n", "\n") -- normalize line endings

	for _, asset in ipairs(response.assets) do
		if asset.name == FAVORED_RELEASE_ASSET then
			-- must be the browser_download_url so that we can immediately download it
			release.downloadURL = asset.browser_download_url
			break
		end
	end

	if not release.downloadURL then
		return nil
	end

	return release
end

--- Retrieves the latest release information from the GitHub API
---@param onSuccess fun(release: GellyParsedRelease) The callback to be called when the release information is successfully retrieved
---@return nil
local function getLatestRelease(onSuccess)
	http.Fetch(GH_RELEASES_API_URL, function(body, _, _, code)
		if code ~= 200 then
			error("Failed to fetch latest release information: HTTP " .. code)
		end

		local release = parseReleaseResponse(body)
		if not release then
			return
		end

		onSuccess(release)
	end, function(err)
		error("Failed to fetch latest release information: " .. err)
	end)
end

--- Checks if the given release is different than the current version of Gelly
---@param release GellyParsedRelease The release to check
---@return boolean different True if the release is different than the current version, false otherwise
local function isReleaseDifferentThanCurrentVersion(release)
	return release.version == gelly.GetVersion()
end

hook.Add("GellyLoaded", "gelly.check-for-updates", function()
	getLatestRelease(function(release)
		if isReleaseDifferentThanCurrentVersion(release) then
			local popup = vgui.Create("GellyNewVersionPopup")
			popup:SetPopupTitle(("Gelly %s is available!"):format(release.version))
			popup:SetMarkdown(release.releaseNotes)
			popup:SetAction("Download")
			popup:SetOnAction(function()
				gui.OpenURL(release.downloadURL)
			end)

			popup:Popup()
		end
	end)
end)
