# Gelly Mods

Gelly mods are self-contained GMod addons (or built-in Lua files) which register to Gelly and provide additional functionality to Gelly. This sort of registering is required to ensure the best compatibility with Gelly and to prevent conflicts with other mods.

# Entities and Weapons

These types of mods don't need to be registered or combined with the mod manager, but should attempt to do so if they want to be compatible with other mods. This is because some mods may change a global setting and cause a runtime issue if the mod is assuming the setting is the default.