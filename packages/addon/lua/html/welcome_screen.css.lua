@import url('https://fonts.googleapis.com/css2?family=Kanit:ital,wght@0,100;0,200;0,300;0,400;0,500;0,600;0,700;0,800;0,900;1,100;1,200;1,300;1,400;1,500;1,600;1,700;1,800;1,900&display=swap');

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
	width: 300px;
	/* initial animation, swoop in from the top and fade in */
	animation: swoop var(--logo-animation-time) ease 1;
	animation-iteration-count: 1;
}

#welcome-text {
	padding: 10px;
	width: 30%;
	background: rgba(0, 0, 0, 0.13);
	border-radius: 10px;
	display: flex;
	flex-direction: column;
	align-items: center;
	justify-content: space-around;

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
	background: #0e90d2;
	color: white;
	font-family: "Kanit", sans-serif;
	font-size: 1em;

	border: 2px solid #0e90d2;
	border-radius: 5px;

	transform: scale(1);
	transition: transform 0.2s ease;

	will-change: transform;
}

#close:hover {
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
	font-family: "Kanit", sans-serif;
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
	font-family: "Kanit", sans-serif;
	font-size: 1em;
	font-weight: 200;
	font-style: normal;
}

h1 {
	color: white;
	font-family: "Kanit", sans-serif;
	font-size: 2em;
	font-weight: 600;
	font-style: normal;
}

p {
	color: white;
	font-family: "Kanit", sans-serif;
	font-size: 1em;
	font-weight: 200;
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