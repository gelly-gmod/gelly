# Developer Guide: Gelly's UI

> [!IMPORTANT]
> As of now, there exists a set of legacy UI modules which are written in raw HTML, CSS, and JavaScript. These modules
> do not
> require any of the modern web development tools, and can be edited directly. However, these modules are deprecated, do
> not
> write new modules in this style. New modules are output to `gelly-ui`.

You might be wondering why there is no UI during local development. This is because you need to have
a basic web-development environment to work on the UI.

## Quick Start

If all you need to do is get the UI back, just ensure you've installed [Node.js](https://nodejs.org/en/), version
v20.1.0 and higher are recommended.

Then, run the following commands:

```bash
cd packages/addon-ui
npm install
npm run build:gmod
```

If all goes well, you'll see Vite transform modules, then a script named "gmod-fix.js" run, and finally it should
terminate on that script. If you see this, you're good to go, and Gelly will automatically load the new UI files on the
next game restart.

## Development

Continuing from the Quick Start, you can run the following command to start a development server:

```bash
cd packages/addon-ui
npm run dev
```

Ensure that you see Vite's development server running on `localhost:80`.
It is imperative that you have it running on this port, as Gelly will look for the UI on this port due to Garry's Mod's
limitations.

Next, ensure that you have the Garry's Mod console open, and run the following command:

```lua
gelly_ui_point_to_localhost 1
```

This will cause every future game restart to point the UI to your local development server. It's worth noting that by
default
you won't see hot reloading. There's no clean work-around for this, you can try patching GMod's CEF files
with [GModCEFCodecFix](https://github.com/solsticegamestudios/GModCEFCodecFix),
but do be warned you will mismatch your CEF version with our intended version.