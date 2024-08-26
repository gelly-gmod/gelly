function setupGModLinks() {
	const links = document.querySelectorAll('.social-link');
	links.forEach(link => {
		link.addEventListener('click', () => {
			const url = link.dataset.gmodUrl;
			if (url) {
				gelly.openURL(url);
			}
		});
	});
}

function setupCloseButton() {
	const closeButton = document.getElementById('close');
	closeButton.addEventListener('click', () => {
		gelly.closeWelcomeScreen();
	});
}

document.addEventListener('DOMContentLoaded', () => {
	setupGModLinks();
	setupCloseButton();
});