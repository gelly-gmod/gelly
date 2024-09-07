<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<title>Gelly Customization</title>
	<script type="text/javascript" src="asset://garrysmod/lua/html/util/load_stylesheet.js.lua"></script>
	<script type="text/javascript" src="asset://garrysmod/lua/html/components/explanation.js.lua"></script>
	<script type="text/javascript" src="asset://garrysmod/lua/html/components/label-slider.js.lua"></script>
	<script type="text/javascript" src="asset://garrysmod/lua/html/customization/graphics.js.lua"></script>
	<script type="text/javascript" src="asset://garrysmod/lua/html/customization/simulation.js.lua"></script>

	<script>
		const PAGE_TO_ELEMENT_MAP = {
			presets: "h1",
			graphics: "gelly-graphics-page",
			mods: "h1",
			simulation: "gelly-simulation-page"
		}

		function setCurrentPage(button) {
			const pageButtons = document.querySelectorAll("#pages .page");
			const pageId = button.dataset.pageId;
			const currentPage = document.querySelector("#currentPage");

			// query for the page element, and hide everything else
			currentPage.querySelectorAll("*").forEach(e => e.style.display = "none");
			const pageElement = currentPage.querySelector(PAGE_TO_ELEMENT_MAP[pageId]);
			if (!pageElement) {
				currentPage.appendChild(document.createElement(PAGE_TO_ELEMENT_MAP[pageId]));
			} else {
				pageElement.style.display = "block";
			}

			// mark this as the active page
			pageButtons.forEach(b => b.classList.remove("active"));
			button.classList.add("active");
		}

		document.addEventListener("DOMContentLoaded", () => {
			const pageButtons = document.querySelectorAll("#pages .page");

			pageButtons.forEach(button => {
				button.addEventListener("click", () => {
					setCurrentPage(button);
				});
			});

			setCurrentPage(pageButtons[0]);

			const closeButton = document.querySelector("#close");
			closeButton.addEventListener("click", () => {
				gelly.hide();
			});
		});
	</script>

	<style>
		body {
			font-family: sans-serif;
			margin: 0;
			padding: 0;

			width: 100vw;
			height: 100vh;

			overflow: hidden;
			box-sizing: border-box;
		}

		main {
			display: flex;
			flex-direction: column;
			width: 100%;
			height: 100%;
		}

		#pages {
			display: flex;
			flex-direction: row;
			justify-content: flex-start;
			gap: 10px;
			align-items: center;
			padding: 10px;
			width: 100%;
			height: fit-content;

			background: rgba(255, 255, 255, 0.35);
		}

		.page {
			cursor: pointer;
			padding: 10px;
			color: #3c3c3d;
			transition: color 0.2s;

			font-family: sans-serif;
			font-size: 1.2em;
			font-weight: bold;
		}

		.page:hover {
			color: #fff;
		}

		.page.active {
			color: #ccc;
		}

		.spacer {
			flex-grow: 1;
		}

		#close {
			cursor: pointer;
			padding: 10px;
			color: #3c3c3d;
			transition: color 0.2s;

			font-family: sans-serif;
			font-size: 1.2em;
			font-weight: bold;
		}

		#close:hover {
			color: #ff6f6f;
		}
	</style>
</head>
<body>
<main>
	<section id="pages">
		<span class="page" data-page-id="presets">
			PRESETS
		</span>

		<span class="page" data-page-id="graphics">
			GRAPHICS
		</span>

		<span class="page" data-page-id="mods">
			MODS
		</span>

		<span class="page" data-page-id="simulation">
			SIMULATION
		</span>

		<span class="spacer"></span>
		<span id="close">
			CLOSE
		</span>
	</section>

	<section id="currentPage">
		<!-- Dynamically filled with the elements provided by the customization folder -->
	</section>
</main>
</body>
</html>