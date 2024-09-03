<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<title>Gelly Customization</title>
	<script type="text/javascript" src="asset://garrysmod/lua/html/util/load_stylesheet.js.lua"></script>
	<script type="text/javascript" src="asset://garrysmod/lua/html/components/explanation.js.lua"></script>
	<script type="text/javascript" src="asset://garrysmod/lua/html/components/label-slider.js.lua"></script>
	<script type="text/javascript" src="asset://garrysmod/lua/html/customization/graphics.js.lua"></script>
	<script type="text/javascript" src="asset://garrysmod/lua/html/customization/performance.js.lua"></script>

	<script>
		document.addEventListener("DOMContentLoaded", () => {
			const pageMap = {
				presets: "h1",
				graphics: "gelly-graphics-page",
				mods: "h1",
				performance: "gelly-performance-page"
			}

			const pageButtons = document.querySelectorAll("#pages .page");

			pageButtons.forEach(button => {
				button.addEventListener("click", () => {
					const pageId = button.dataset.pageId;
					const currentPage = document.querySelector("#currentPage");

					// query for the page element, and hide everything else
					currentPage.querySelectorAll("*").forEach(e => e.style.display = "none");
					const pageElement = currentPage.querySelector(pageMap[pageId]);
					if (!pageElement) {
						currentPage.appendChild(document.createElement(pageMap[pageId]));
					} else {
						pageElement.style.display = "block";
					}

					// mark this as the active page
					pageButtons.forEach(b => b.classList.remove("active"));
					button.classList.add("active");
				});
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

		<span class="page" data-page-id="performance">
			PERFORMANCE
		</span>
	</section>

	<section id="currentPage">
		<!-- Dynamically filled with the elements provided by the customization folder -->
	</section>
</main>
</body>
</html>