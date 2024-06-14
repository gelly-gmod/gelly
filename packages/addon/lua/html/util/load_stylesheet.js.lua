// unfortunate hack due to the GMAD whitelist not allowing HTML/CSS/JS files

const isCustomStylesheetElement = elem => elem.tagName === "LINK" && elem.rel === "stylesheet" && elem.dataset.bypassGmodWhitelist === "true";

function loadStylesheet(originalStylesheetLink) {
	let head = document.getElementsByTagName('head')[0];
	let link = document.createElement('link');

	let request = new XMLHttpRequest();
	request.open('GET', originalStylesheetLink.href, false);
	request.send(null);

	link.rel = 'stylesheet';
	link.type = 'text/css';
	link.href = 'data:text/css,' + encodeURIComponent(request.responseText);
	link.media = 'all';

	head.appendChild(link);
}

Array.from(document.getElementsByTagName('link')).filter(isCustomStylesheetElement).forEach(loadStylesheet);
