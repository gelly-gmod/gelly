import { defineConfig } from "vite";
import { preact } from "@preact/preset-vite";
import { resolve } from "path";
import { viteSingleFile } from "vite-plugin-singlefile";

export default defineConfig({
	plugins: [preact(), viteSingleFile()],
	root: "ui",
	build: {
		outDir: "../../addon/lua/html/gelly-ui",
		emptyOutDir: true,
		rollupOptions: {
			input: {
				customization: resolve(__dirname, "ui/customization.html"),
			},
		},
	},
	server: {
		port: 80,
		strictPort: true,
	},
});
