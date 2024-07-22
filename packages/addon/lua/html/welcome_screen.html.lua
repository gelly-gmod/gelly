<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<title>Welcome to Gelly</title>

	<link rel="stylesheet" type="text/css" href="asset://garrysmod/lua/html/welcome_screen.css.lua"
		  data-bypass-gmod-whitelist="true"/>

	<link rel="stylesheet" type="text/css" href="asset://garrysmod/lua/html/gelly_theme.css.lua"
		  data-bypass-gmod-whitelist="true"/>

	<script type="text/javascript" src="asset://garrysmod/lua/html/util/load_stylesheet.js.lua"></script>
	<script type="text/javascript" src="asset://garrysmod/lua/html/welcome_screen.js.lua"></script>
</head>
<body>
<main>
	<!-- I'm not even joking, this is easier to implement than actually packaging the png locally -->
	<img src="https://raw.githubusercontent.com/gelly-gmod/gelly/master/branding/gellylogo.png" alt="Gelly Logo"
		 id="logo"/>

	<article id="welcome-text">
		<h1>Welcome!</h1>

		<p>If you can see this, that means Gelly was properly installed. No further steps are required. Once you exit
			this screen, it won't be shown again.</p>
		<p>Enjoy!</p>
		<button id="close">Close</button>

		<section id="socials">
			<span class="social-link" data-gmod-url="https://github.com/gelly-gmod/gelly">GitHub</span>
			<span class="separator">•</span>
			<span class="social-link" data-gmod-url="https://discord.com/invite/vmBvxvawYS">Discord</span>
		</section>
	</article>
</main>
</body>
</html>