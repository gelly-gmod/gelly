<p align="center">
   <img src="branding/gellylogo.png" border="20" />
   <br/>
   <a href="https://github.com/yogwoggf/gelly/releases">Grab the GMod addon</a>
</p>

<br/>

> [!IMPORTANT]
> Gelly is *free open-source software*. It is provided **without warranty or support**.
> There is no obligation to provide support or updates for Gelly. You are free to use, modify, and distribute Gelly as
> you see fit according to the license.

Welcome to Gelly, a primarily Garry's Mod addon which enables sandbox-oriented fluid simulation.

- [Features](#features)
- [Installation](#installation)
- [Bugs and Issues](#bugs-and-issues)
- [Contributing](#contributing)
- [License](#license)
- [Acknowledgements](#acknowledgements)

# Features

Gelly is equipped with a high-performance fluid rendering system to deliver a smooth and visually appealing look.
The fluid simulation uses [FleX](https://developer.nvidia.com/flex) as its backend, which is a particle-based physics
engine that has a game-oriented fluid simulation system.

A few of the notable features of Gelly include:

- Fluid simulation
- Realistic fluid rendering
    - As much as Garry's Mod allows, that is
- Customizable fluid properties
    - Viscosity, surface tension, etc.
    - Fluid absorption (yes, **absorption**, not color)
        - All fluids are described by their absorption coefficients for red, green, and blue light,
        - See [this page](https://en.wikipedia.org/wiki/Beer%E2%80%93Lambert_law) for more information
        - In other words, fluid color is obtained by describing how much light is absorbed by the fluid, for each color
          channel (red, green, blue)
    - Presets
        - Water, piss, blood, Gell-O and glunk.
            - Gell-O is a gelatinous fluid. There is no intention for a coincidential name with a popular American brand
              of gelatin desserts.
            - Glunk is an odd, diffuse fluid which acts like a mix of concrete and water, you can't see through it, but
              it
              acts very fluidly.
    - Physically inspired material rendering system
        - Roughness, specular transmission and absorption
    - Fluids can be mixed (**but not materials**)
        - This means that a red fluid and a blue fluid will mix to form a dark purple fluid,
        - but a red fluid and a blue opaque material will not mix, they will just overlap.
- Fluid interaction with the environment
    - Currently ragdolls aren't supported
    - Two-way interaction is not supported
        - More research has to be done and this is likely going to not be implemented due to the general incompatability
          between orthodox physics engines and FleX's particle-based system
- Time control
    - You can pause, slow down, or speed up the simulation
- The Gelly Gun
    - A tool that allows you to spawn fluids, the main entry point for spawning fluids

> [!NOTE]
> Time control is not perfect, and it is not guaranteed to work as expected. It *can* change the results of the
> simulation, such as
> things like Gell-O just falling and not holding its shape. This is a limitation of the FleX backend, and it is not
> something
> that can be fixed easily.

# Installation

It's simple, just go over to the releases page and download the latest version of Gelly. Extract the contents of the
archive to your Garry's Mod folder, the one opened by Steam when you click on "Browse local files" in the game's menu.

This is also reiterated in the releases.

> [!IMPORTANT]
> Gelly requires a 64-bit operating system to run. If you are running a 32-bit operating system, Gelly will not work.
> Additionally, Gelly requires that you run GMod in 64-bit mode. If you are running GMod in 32-bit mode, Gelly will not
> run. Another important note is that Gelly does require GMod to be running in D3D9Ex mode. This is usually always the
> case, but if you have a launch parameter of `-nod3d9ex`, Gelly will bail on first load.
>
> Gelly requires a GPU that supports DirectX 11.4. If you're unsure, most GPUs made past 2012 should
> support it.

# Bugs and Issues

If you encounter any bugs or issues, please report them on
the [issue tracker](https://github.com/yogwoggf/gelly/issues).
Please provide as much information as possible, and do not try to free-hand the issue.

The provided forms are there for a reason, and they help me understand the issue better.

> [!IMPORTANT]
> Again, Gelly is *my* project, and I am not obligated to provide support or updates for it.
> However, I will try to fix any issues that are reported.

## How to retrieve your log

Gelly produces highly detailed, but not personally identifiable logs. These logs are stored in the root folder of
Garry's Mod,
like the one you extracted Gelly to. The log is named `gelly-date.log`, where `date` is the date of the game session.

# Contributing

Please hold off on contributing code/features until the project is more mature. PRs are welcome, but they will likely be
rejected
until the project is more mature. Right now, style guide and code linting are not enforced, which is a big blocker for
PRs.

However, suggestions and bug reports are always welcome.

# License

Gelly is licensed under the GPL v3.0 license. You can find the full license text in the `LICENSE` file.

I chose this license because of the passion project nature of Gelly, and I want to ensure that it remains open-source
in case anyone wants to continue development once I inevitably abandon it.

> [!NOTE]
> Gelly is not that serious of a project, there's no worries if you just want to steal the code and rebrand it.
> Nobody will care, but I do like a little credit.

# Acknowledgements

Gelly would not be possible without the following projects and people:

- [Garry's Mod](https://store.steampowered.com/app/4000/Garrys_Mod/)
- [FleX](https://developer.nvidia.com/flex)
- The *amazing* GMod libraries provided by [Derpius](https://github.com/Derpius)
    - [GMFS](https://github.com/Derpius/GMFS)
    - [BSPParser](https://github.com/Derpius/BSPParser)
- Plenty of ghostwriting from [Derpius](https://github.com/Derpius)
- Architectural, product and design advice from [Derpius](https://github.com/Derpius)
    - Plenty of advice on how to structure the project and do product planning, including some hands-on help
      at [Total Anarchy Servers](https://github.com/TAServers)
        - Great GMod servers
- Notable criticisms and suggestions for the fluid rendering system from [Derpius](https://github.com/Derpius)
- Well-needed call-outs for performance regressions from [Derpius](https://github.com/Derpius)
- Support from [Meetric](https://github.com/meetric1) on our commonly shared issues facing FleX and GMod
- Meetric's original GWater addon, which inspired Gelly and eventually turned it into a highly educational project
    - Consider donating for his new GWater2 addon: [Meetric's Patreon](https://www.patreon.com/meetric)
- The patience of the [Graphics Programming Discord](https://discord.com/invite/Eb7P3wH) members
- [RenderDoc](https://renderdoc.org/)