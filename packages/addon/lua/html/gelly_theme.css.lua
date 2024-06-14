:root {
	--button-border-radius: 5px;
	--button-border-size: 2px;
	--button-press-scale: 0.95;
}

button {
	background-color: black;
	color: white;
	font-family: "Kanit", sans-serif;
	font-size: 1em;
	font-weight: 600;
	width: 100px;

	border: var(--button-border-size) solid rgb(36, 36, 36);
	border-radius: var(--button-border-radius);
	outline: none;

	transition: background-color 0.2s, color 0.2s, transform 0.2s;
}

button:hover {
	background-color: white;
	color: black;
}

button:active {
	transform: scale(var(--button-press-scale));
}
