# Gelly Mods

Gelly mods are self-contained GMod addons (or built-in Lua files) which register to Gelly and provide additional
functionality to Gelly. This sort of registering is required to ensure the best compatibility with Gelly and to prevent
conflicts with other mods.

## Creating a Mod

You should follow the generic steps for making a GMod addon, but place your mod-specific code
in `lua/gelly/mods/<YOUR_MOD_NAME_HERE>` so that it can be easily identified and managed by Gelly.

### Mod Structure

All mods should have the following structure:

- `init.lua` - The main entry point for your mod. This is where you should perform your mods tasks. All GellyX
  facilities are ready and available at this point.
- `info.lua` - A file containing metadata about your mod. This is used by Gelly to display information about your mod in
  the mod manager and to ensure compatibility with other mods. See the example mods for more information.

> [!NOTE]
> `init.lua` is **clientside**. It makes no sense to have a serverside `init.lua` for a Gelly mod, however you are free
> to
> create a serverside portion of your addon if you wish. An example of this is the serverside damage system in GellyX
> which
> allows clientside mods to see damage information (used by the blood mod).

# Entities and Weapons

These types of mods don't need to be registered or combined with the mod manager, but should attempt to do so if they
want to be compatible with other mods. This is because some mods may change a global setting and cause a runtime issue
if the mod is assuming the setting is the default.