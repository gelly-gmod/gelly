include("gx-settings.lua")

include("gx-soft-functions.lua")
include("gx-forcefields.lua")
include("gx-fluid-presets.lua")
gellyx.presets.loadPresetFiles()

include("gx-replicate-damage.lua")
include("gx-replicate-bullets.lua")
include("gx-replicate-explosions.lua")
include("gx-replicate-removal.lua")
include("gx-replicate-buttons.lua")

include("emitters/cube-emitter.lua")
include("emitters/mesh-emitter.lua")
include("emitters/sphere-emitter.lua")

include("mods/enums.lua")
include("mods/manager.lua")

include("gelly/util/prop-resizer.lua")

gellyx.mods.initialize()
gellyx.mods.runMods()

include("gx-settings-sync.lua")
gellyx.settings.updateBinaryModuleSettings()

include("gelly/ui/customization.lua")
GELLY_CUSTOMIZATION = vgui.Create("GellyCustomizationMenu")
GELLY_CUSTOMIZATION:Hide()
