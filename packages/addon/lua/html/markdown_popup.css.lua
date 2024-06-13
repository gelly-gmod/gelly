@import url('https://fonts.googleapis.com/css2?family=Kanit:ital,wght@0,100;0,200;0,300;0,400;0,500;0,600;0,700;0,800;0,900;1,100;1,200;1,300;1,400;1,500;1,600;1,700;1,800;1,900&display=swap');

body {
	display: flex;
	flex-direction: column;

	padding: 25px;
	margin: 0;

	height: calc(100vh - 25px);

	overflow: hidden;
	background: rgba(0, 0, 0, 0);
}

header {
	display: flex;
	min-height: 32px;
}

header h1 {
	color: white;
	margin: 0;

	font-family: "Kanit", sans-serif;
	font-size: 2em;
	font-weight: 600;
	font-style: normal;
}

main {
	display: flex;
	flex-direction: column;
	justify-content: flex-start;
	height: 100%;

	/* at a certain point we do not want it to push the buttons down */
	overflow: auto;
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

ul {
	font-family: "Kanit", sans-serif;
	font-size: 1em;
	font-weight: 400;
	font-style: normal;

	color: white;
}

h3 {
	margin-left: 3px;

	font-family: "Kanit", sans-serif;
	font-size: 1.5em;
	font-weight: 600;
	font-style: normal;

	color: white;
}
