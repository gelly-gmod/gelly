import { defineConfig } from "vite";
import react from "@vitejs/plugin-react";

export default defineConfig(({ command }) => {
	if (command === "serve") {
		return {
			plugins: [react()],
		};
	} else {
		return {
			plugins: [react()],
			base: "REPLACEASSETURL", // required for Garry's Mod
			build: {
				outDir: "../html/gelly",
			},
		};
	}
});
