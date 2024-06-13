const VALID_HEADERS = ['Added', 'Changed', 'Fixed', 'Deprecated', 'Removed', 'Security'];

/**
 * The order in which the headers should be displayed, according to Keep a Changelog guidelines.
 * @type {string[]}
 */
const DISPLAY_HEADER_ORDER = [
	'added',
	'changed',
	'deprecated',
	'removed',
	'fixed',
	'security'
]

const LINE_TYPE = {
	HEADER: 'HEADER',
	BULLET: 'BULLET',
	UNKNOWN: 'UNKNOWN'
}

function getHeader(line) {
	const regexCapture = /### (.+)/;
	const match = regexCapture.exec(line);

	if (match) {
		// first element is just going to be the entire line, so we want the second element
		return match[1];
	}

	return undefined
}

function getBullet(line) {
	const regexCapture = /- (.+)/;
	const match = regexCapture.exec(line);

	if (match) {
		return match[1];
	}

	return undefined
}

function isHeader(line) {
	return line.startsWith('### ');
}

function isBullet(line) {
	return line.startsWith('- ');
}

function getLineType(line) {
	if (isHeader(line)) {
		return LINE_TYPE.HEADER;
	} else if (isBullet(line)) {
		return LINE_TYPE.BULLET;
	}

	return LINE_TYPE.UNKNOWN;
}

/**
 * Parses a subset of Markdown syntax used in the changelog.
 * @param text
 * @returns {{security: string[], removed: string[], added: string[], deprecated: string[], fixed: string[], changed: string[]}}
 */
function parseChangelogMarkdown(text) {
	console.log("text: ", text);
	let lines = text.split('\n');
	let currentSection = undefined;

	let parsed = {
		added: [],
		changed: [],
		fixed: [],
		deprecated: [],
		removed: [],
		security: []
	};

	do {
		const line = lines.shift();

		if (line === undefined) {
			break;
		}

		switch (getLineType(line)) {
			case LINE_TYPE.HEADER:
				const header = getHeader(line);

				if (VALID_HEADERS.includes(header)) {
					currentSection = header.toLowerCase();
				} else {
					currentSection = undefined;
				}

				break;
			case LINE_TYPE.BULLET:
				const bullet = getBullet(line);

				if (currentSection !== undefined && bullet !== undefined) {
					parsed[currentSection].push(bullet);
				}

				break;
			default:
				break;
		}
	} while (lines.length > 0 && currentSection !== undefined);

	return parsed;
}

function createVisualChangelog(parentElement, parsedChangelog) {
	DISPLAY_HEADER_ORDER.filter(header => parsedChangelog[header].length > 0).forEach(header => {
		const headerElement = document.createElement('h3');
		headerElement.innerText = header.charAt(0).toUpperCase() + header.slice(1);

		const listElement = document.createElement('ul');

		parsedChangelog[header].forEach(bullet => {
			const bulletElement = document.createElement('li');
			bulletElement.innerText = bullet;

			listElement.appendChild(bulletElement);
		});

		parentElement.appendChild(headerElement);
		parentElement.appendChild(listElement);
	})
}

function intializeTitleText() {
	const titleElement = document.getElementById('title');
	popup.getTitle(function (titleText) {
		titleElement.innerText = titleText;
	});
}

function initializeActionText() {
	const actionButton = document.getElementById('action');
	popup.getAction(function (actionText) {
		actionButton.innerText = actionText;
	});
}

function setupCloseButton() {
	const closeButton = document.getElementById('close');
	closeButton.addEventListener('click', function () {
		popup.close();
	});
}

function setupActionButton() {
	const actionButton = document.getElementById('action');
	actionButton.addEventListener('click', function () {
		popup.performAction();
	});
}

document.addEventListener('DOMContentLoaded', function () {
	let markdownElement = document.getElementById('content');
	// GMod only allows fetching the return value by passing a callback, so we'll
	// have to do everything in this callback.

	popup.getMarkdown(function (markdownFromAddon) {
		let parsedChangelog = parseChangelogMarkdown(markdownFromAddon);

		createVisualChangelog(markdownElement, parsedChangelog);

		intializeTitleText();
		initializeActionText();

		setupCloseButton();
		setupActionButton();
	});
});