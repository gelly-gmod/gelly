<!DOCTYPE html>
<html>
<head>
	<title>Preset Creator</title>
	<link rel="stylesheet" type="text/css" href="asset://garrysmod/lua/html/preset_creator.css.lua"
		  data-bypass-gmod-whitelist="true"/>

	<link rel="stylesheet" type="text/css" href="asset://garrysmod/lua/html/gelly_theme.css.lua"
		  data-bypass-gmod-whitelist="true"/>

	<script type="text/javascript" src="asset://garrysmod/lua/html/util/load_stylesheet.js.lua"></script>
	<script type="text/javascript" src="asset://garrysmod/lua/html/preset_creator.js.lua"></script>
</head>
<body>
<header>
	<h1>Gelly Preset Creator</h1>
</header>

<main>
	<section id="top-section">
		<section class="setting">
			<input type="text" id="preset-name" placeholder="Preset Name"/>
			<label for="preset-name">Preset Name</label>
		</section>
		<section class="setting">
			<input type="range" id="preset-size" min="0.5" max="5" step="0.1" value="1"/>
			<label for="preset-size">Particle Radius</label>
		</section>
	</section>

	<!-- Contains all the fun stuffs to tune -->
	<section id="middle-section">
		<section class="setting">
			<input type="range" id="preset-stickiness" min="0" max="0.2" step="0.001" value="0"/>
			<label for="preset-stickiness">Stickiness</label>
		</section>

		<section class="setting">
			<input type="range" id="preset-cohesion" min="0" max="1" step="0.1" value="0"/>
			<label for="preset-cohesion">Cohesion</label>
		</section>

		<section class="setting">
			<input type="range" id="preset-surface-tension" min="0" max="0.01" step="0.0001" value="0"/>
			<label for="preset-surface-tension">Surface Tension</label>
		</section>

		<section class="setting">
			<input type="range" id="preset-gloopiness" min="0" max="100" step="1" value="0"/>
			<label for="preset-gloopiness">Gloopiness</label>
		</section>

		<section class="setting">
			<input type="range" id="preset-friction" min="0" max="5" step="0.5" value="0"/>
			<label for="preset-friction">Friction</label>
		</section>

		<section class="setting">
			<input type="checkbox" id="preset-foam"/>
			<label for="preset-foam">Foam</label>
		</section>
	</section>

	<!-- All graphical stuffs -->
	<section id="bottom-section">
		<section class="setting">
			<input type="range" id="preset-roughness" min="0" max="1" step="0.05" value="0"/>
			<label for="preset-roughness">Roughness</label>
		</section>

		<section class="setting">
			<input type="range" id="preset-refractive-index" min="1" max="2" step="0.1" value="1"/>
			<label for="preset-refractive-index">Refractive Index</label>
		</section>

		<section class="setting">
			<input type="color" id="preset-color" value="#FFFFFF"/>
			<label for="preset-color">Color</label>
		</section>

		<section class="setting">
			<input type="range" id="preset-color-strength" min="0" max="1" step="0.001" value="1"/>
			<label for="preset-color-strength">Color Strength</label>
		</section>

		<section class="setting">
			<input type="checkbox" id="preset-opaque" value="false"/>
			<label for="preset-opaque">Opaque</label>
		</section>
	</section>
</main>

<footer>
	<button id="preset-save">Save</button>
	<button id="preset-test">Test</button>
</footer>
</body>
</html>