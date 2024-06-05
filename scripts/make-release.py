import logging
import os
import shutil
import stat
import subprocess
import time

logger = logging.getLogger("make-release")

GELLY_PRESET = "Gelly-GMod-Release"
GELLY_BINARY_DIRECTORY = "bin/" + GELLY_PRESET.lower() + "/packages/gelly-gmod/"
GELLY_DLL_NAME = "gmcl_gelly-gmod_win64.dll"
GELLY_PDB_NAME = "gmcl_gelly-gmod.pdb"
GELLY_RELEASE_ZIP_NAME = "gelly-production-release"
GELLY_FLEX_DIRECTORY = "packages/gelly/modules/gelly-fluid-sim/vendor/FleX/bin/win64/"
GELLY_FLEX_DEPENDENCIES = [
    "nvToolsExt64_1.dll",
    "amd_ags_x64.dll",  # both critical for gpu extension support
    "GFSDK_Aftermath_Lib.x64.dll",
    # FleX masks all D3D calls with Aftermath, so it's critical to include--even if the GPU is not Nvidia

    "NvFlexDeviceRelease_x64.dll",
    "NvFlexExtReleaseD3D_x64.dll",
    "NvFlexReleaseD3D_x64.dll"
]

RELEASE_README = """
MANUAL INSTALLATION STEPS:
1. Select all files from inside this directory
2. Drag and drop to your GMod folder (the one in steamapps/common, the one that has a "garrysmod" folder)
    a. You can alternatively copy the files and go into the GMod folder and then paste
3. You Are Done! Yay!!!!!
"""

LOADER_LUA_SCRIPT = """game.MountGMA("data/gelly.gma")"""

logging.basicConfig()
logging.root.setLevel(logging.NOTSET)


def bail_if_not_in_dev_cmd():
    if "VSCMD_VER" not in os.environ:
        logger.error("rerun this script in a VS command line")
        exit(1)


def make_release_dir():
    logger.info("creating release directory")
    os.makedirs("release", exist_ok=True)
    os.makedirs("release/garrysmod/addons/", exist_ok=True)
    os.makedirs("release/garrysmod/lua/bin/", exist_ok=True)


def configure_gelly():
    logger.info("configuring gelly")

    try:
        subprocess.run(
            ['cmake', '-DGELLY_PRODUCTION_BUILD=ON', '--fresh', '--preset ' + GELLY_PRESET.lower(), '-S .',
             '-B bin/' + GELLY_PRESET.lower()],
            check=True,
            stdout=subprocess.DEVNULL)
    except subprocess.CalledProcessError as e:
        logger.error(f"cmake configuration failed! (error code: {e.returncode})")


def build_gelly():
    logger.info("building gelly (may take some time)")

    try:
        subprocess.run(
            ['cmake', '--build', '--target gelly-gmod', '--preset ' + GELLY_PRESET, '--clean-first'],
            check=True,
            stdout=subprocess.DEVNULL
        )
    except subprocess.CalledProcessError as e:
        logger.error(f"failed to compile gelly via cmake (error code: {e.returncode})")


def build_dll():
    logger.info("creating " + GELLY_PRESET + " binary (gmcl_gmod-gelly_win64.dll)")
    configure_gelly()
    build_gelly()


def move_dll_to_release():
    logger.info("moving dll to release directory")
    os.replace(f"{GELLY_BINARY_DIRECTORY}{GELLY_DLL_NAME}", f"release/garrysmod/lua/bin/{GELLY_DLL_NAME}")
    if GELLY_PRESET != "Gelly-GMod-Release":
        os.replace(f"{GELLY_BINARY_DIRECTORY}{GELLY_PDB_NAME}", f"release/garrysmod/lua/bin/{GELLY_PDB_NAME}")


def insert_copy_of_gelly_addon():
    logger.info("cloning gelly addon into release")
    try:
        subprocess.run(
            "git clone --depth=1 -b sp-addon https://github.com/yogwoggf/gelly.git ./release/garrysmod/addons/gelly",
            shell=True,
            check=True,
            stdout=subprocess.DEVNULL
        )
    except subprocess.CalledProcessError as e:
        logger.error(f"failed to checkout a copy of the gelly addon (error code: {e.returncode}")


def strip_git_dir_from_addon():
    logger.info("stripping away .git from the addon")
    logger.info("prepping git files for destruction...")
    for root, _, files in os.walk("release/garrysmod/addons/gelly/.git"):
        for file in files:
            os.chmod(os.path.join(root, file), stat.S_IWUSR)
    logger.info("successfully primed git files for removal")

    shutil.rmtree("release/garrysmod/addons/gelly/.git")


def bundle_addon_to_gma():
    logger.info("bundling addon into gma file")
    # create the data dir tree first
    os.makedirs("release/garrysmod/data", exist_ok=True)

    try:
        subprocess.run(
            "scripts\\gmad.exe create -folder \"release\\garrysmod\\addons\\gelly\" -out \"release\\garrysmod\\data\\gelly.gma\" -icon \"release\\garrysmod\\addon\\gelly\\logo.jpg\"",
            shell=True,
            check=True
        )
    except subprocess.CalledProcessError as e:
        logger.error(f"failed to bundle gelly (error code: {e.returncode})")


def destroy_addon_directory():
    logger.info("destroying addon tree")
    shutil.rmtree("release/garrysmod/addons/gelly")


def create_loader_addon():
    logger.info("adding bootstrap addon")
    os.makedirs("release/garrysmod/addons/gelly_bootstrap/lua/autorun")
    with open("release/garrysmod/addons/gelly_bootstrap/lua/autorun/load_gelly.lua", "w+") as file:
        file.write(LOADER_LUA_SCRIPT)


def copy_flex_dependencies():
    logger.info("copying flex dependences into release archive")

    for dep in GELLY_FLEX_DEPENDENCIES:
        dependency_path = os.path.join(GELLY_FLEX_DIRECTORY, dep)
        new_path = os.path.join("release/", dep)

        logger.info(f"copying '{dep}")
        shutil.copyfile(dependency_path, new_path)


def make_readme_file():
    logger.info("adding readme to release directory")
    with open("release/README.txt", "w") as readme_file:
        readme_file.write(RELEASE_README)


def create_release_archive():
    logger.info("creating release archive")
    shutil.make_archive(GELLY_RELEASE_ZIP_NAME, "zip", "release")


def remove_release_directory():
    logger.info("cleaning up...")
    shutil.rmtree("release")


def make_release():
    make_release_dir()
    build_dll()
    move_dll_to_release()
    insert_copy_of_gelly_addon()
    strip_git_dir_from_addon()
    bundle_addon_to_gma()
    destroy_addon_directory()
    create_loader_addon()
    copy_flex_dependencies()
    make_readme_file()
    create_release_archive()
    remove_release_directory()
    logger.info("done!")


bail_if_not_in_dev_cmd()
make_release()
