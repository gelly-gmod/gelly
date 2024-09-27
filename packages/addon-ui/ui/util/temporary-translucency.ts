export function enableTemporaryTranslucency() {
	gelly.getActiveParticles((count) => {
		if (count <= 0) {
			return;
		}

		document.body.classList.add("translucent");
	});
}

export function disableTemporaryTranslucency() {
	document.body.classList.remove("translucent");
}
