@import url('https://fonts.googleapis.com/css2?family=Kanit:ital,wght@0,100;0,200;0,300;0,400;0,500;0,600;0,700;0,800;0,900;1,100;1,200;1,300;1,400;1,500;1,600;1,700;1,800;1,900&display=swap');

body {
	display: flex;
	flex-direction: column;
	gap: 10px;

	padding: 0px 25px 0px 25px;
	margin: 0;

	height: 100vh;

	overflow: hidden;
}

header {
	display: flex;
	min-height: 64px;
}

header h1 {
	color: white;
	margin: 0;

	font-family: "Kanit", sans-serif;
	font-size: 1.5em;
	font-weight: 600;
	font-style: normal;
}

main {
	display: flex;
	flex-direction: column;
	justify-content: space-between;
	height: 100%;
}

footer {
	padding: 10px;

	display: flex;
	flex-direction: row;
	flex-wrap: wrap;

	justify-content: flex-end;
	align-items: center;
	gap: 30px;

	width: 100%;
}

#top-section {
	display: flex;
	flex-direction: row;
	/* allow to flow down in case of overflow */
	flex-wrap: wrap;
	justify-content: space-between;
	align-items: center;
}

#middle-section {
	display: flex;
	flex-direction: row;
	justify-content: space-between;
	align-items: center;
	gap: 10px;
	flex-wrap: wrap;
}

#bottom-section {
	display: flex;
	flex-direction: row;
	justify-content: space-between;
	align-items: center;
	gap: 10px;
	flex-wrap: wrap;
}

.setting {
	display: flex;
	min-width: 200px;
	flex-direction: column;
	gap: 5px;

	transition: transform 0.2s, text-shadow 0.2s;
}

.setting:hover {
	transform: scale(1.05);
	text-shadow: 0px 0px 5px white;
}

.setting label {
	color: white;
	font-family: "Kanit", sans-serif;
	font-size: 1em;
	font-weight: 200;
	font-style: normal;

	padding: 0;
	margin: 0;
	text-align: center;
}

.setting input[type="color"] {
	width: 100%;
	height: 30px;

	/* get rid of the ugly border */
	border: none;
	background: none;
}

.setting input[type="text"] {
	width: 100%;
	height: 30px;

	/* get rid of the ugly border */
	border: none;
	outline: none;
	background: rgb(36, 36, 36);
	border-radius: 5px;

	color: white;
	font-family: "Kanit", sans-serif;
	font-size: 1em;
	font-weight: 200;
	font-style: normal;

	text-align: center;
}

.setting input[type="checkbox"] {
	width: 100%;
	height: 30px;

	/* get rid of the ugly border */
	border: none;
	outline: none;
	background: none;

	justify-content: center;
	vertical-align: middle;

	/* checkboxes need special styling to be centered */
	margin: 0;
}