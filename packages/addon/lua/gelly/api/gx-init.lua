include("gx-settings.lua")
include("gx-settings-sync.lua")

include("gx-soft-functions.lua")
include("gx-forcefields.lua")
include("gx-fluid-presets.lua")
gellyx.presets.loadPresetFiles()

include("gx-replicate-damage.lua")
include("gx-replicate-removal.lua")

include("emitters/cube-emitter.lua")
include("emitters/mesh-emitter.lua")
include("emitters/sphere-emitter.lua")

include("mods/enums.lua")
include("mods/manager.lua")

gellyx.mods.initialize()
gellyx.mods.runMods()
gellyx.settings.updateBinaryModuleSettings()
