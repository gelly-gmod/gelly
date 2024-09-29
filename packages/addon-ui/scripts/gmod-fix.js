// Rename all files in the gelly-ui folder to have the .lua extension (workaround for Garry's Mod)

const { resolve } = require("path");
const { readFileSync, writeFileSync } = require("node:fs");
const { readdirSync, renameSync } = require("node:fs");

const emitPath = resolve(__dirname, "../../addon/lua/html/gelly-ui");

function fixFileExtension(file) {
	renameSync(file, `${file}.lua`);
}

readdirSync(emitPath, { withFileTypes: true }).forEach((file) => {
	if (file.isDirectory()) return;

	fixFileExtension(resolve(emitPath, file.name));
});
