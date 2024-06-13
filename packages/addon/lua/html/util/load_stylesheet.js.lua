// unfortunate hack due to the GMAD whitelist not allowing HTML/CSS/JS files

function isCustomStylesheetElement(elem) {
	return elem.tagName === "LINK" && elem.rel === "stylesheet" && elem.dataset.bypassGmodWhitelist === "true";
}

function loadStylesheet(elem) {
	let head = document.getElementsByTagName('head')[0];
	let link = document.createElement('link');
	let stylesheetURL = elem.href;

	// request it
	let request = new XMLHttpRequest();
	request.open('GET', stylesheetURL, false);
	request.send(null);

	console.log("load_stylesheet: Requested stylesheet from " + stylesheetURL);

	// insert it
	link.rel = 'stylesheet';
	link.type = 'text/css';
	link.href = 'data:text/css,' + encodeURIComponent(request.responseText);
	link.media = 'all';

	head.appendChild(link);
}

let links = document.getElementsByTagName('link');
for (let i = 0; i < links.length; i++) {
	if (isCustomStylesheetElement(links[i])) {
		loadStylesheet(links[i]);
	}
}