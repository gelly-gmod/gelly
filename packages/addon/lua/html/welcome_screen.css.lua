@import url('https://fonts.googleapis.com/css2?family=Poppins:wght@700&family=Roboto:ital,wght@0,100..900;1,100..900&display=swap');

:root {
	--logo-animation-time: 1.5s;
	--logo-sleep-time: 0.2s;
}

body {
	margin: 0;
	padding: 0;

	width: 100vw;
	height: 100vh;
	overflow: hidden;
}

main {
	width: 100%;
	height: 100%;
	display: flex;
	flex-direction: column;
	align-items: center;
	justify-content: center;
}

@keyframes swoop {
	0% {
		transform: scale(0.9);
		opacity: 0;
	}
	100% {
		transform: scale(1);
		opacity: 1;
	}
}

#logo {
	width: 400px;
	/* initial animation, swoop in from the top and fade in */
	animation: swoop var(--logo-animation-time) ease 1;
	animation-iteration-count: 1;
}

#welcome-text {
	padding: 10px 25px 10px 25px;
	width: 30%;
	background: rgba(0, 0, 0, 0.13);
	border-radius: 10px;
	display: flex;
	flex-direction: column;
	align-items: center;
	justify-content: space-around;
	box-sizing: content-box;
	
	animation: welcome-text-show 0.5s ease 1 forwards;
	animation-iteration-count: 1;
	animation-delay: calc(var(--logo-animation-time) + var(--logo-sleep-time));
	opacity: 0;
}

#close {
	width: 100%;
	height: 38px;
	margin-left: 10px;
	margin-right: 10px;
	margin-bottom: 10px;
	background: #a43939;
	color: white;
	font-family: "Kanit", sans-serif;
	font-size: 1em;

	border: 2px solid #b42f2f;
	border-radius: 5px;

	transform: scale(1);
	transition: transform 0.2s ease;

	will-change: transform;
}

#close:hover {
	cursor: pointer;
	transform: scale(1.02);
}

#socials {
	display: flex;
	flex-direction: row;
	align-items: center;
	justify-content: space-evenly;
	gap: 10px;
}

.social-link {
	color: white;
	font-family: "Roboto", sans-serif;
	font-size: 1em;
	font-weight: 200;
	font-style: normal;

	transition: text-shadow 0.2s;
	cursor: pointer;
}

.social-link:hover {
	text-shadow: 0px 0px 5px white;
}

.separator {
	color: white;
	font-family: "Roboto", sans-serif;
	font-size: 1em;
	font-weight: 200;
	font-style: normal;
}

h1 {
	color: white;
	font-family: "Poppins", sans-serif;
	font-size: 2em;
	font-weight: 600;
	font-style: normal;

	line-height: 1.05em;
}

p {
	color: white;
	font-family: "Roboto", sans-serif;
	font-size: 1em;
	font-weight: 400;
	font-style: normal;

	line-height: 1.2em;
}

li {
	color: white;
	font-family: "Roboto", sans-serif;
	font-size: 1em;
	font-weight: 400;
	font-style: normal;

	line-height: 1em;
}

strong {
	color: white;
	font-family: "Roboto", sans-serif;
	font-size: 1em;
	font-weight: 700;
	font-style: normal;
}

@keyframes welcome-text-show {
	0% {
		transform: translateY(100%);
		opacity: 0;
	}

	100% {
		transform: translateY(0);
		opacity: 1;
	}
}