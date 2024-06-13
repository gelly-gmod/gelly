const MARKDOWN_TO_HTML_REGEX = [
	{
		regex: /### (.+)/g,
		replace: "<h3>$1</h3>"
	},
	{
		regex: /- (.+)/g,
		replace: "<p class='bullet'><span class='bullet-point'>&#x2022;</span> $1</p>"
	}
]

const getMarkdownAsHTML =
	markdownText =>
		MARKDOWN_TO_HTML_REGEX.reduce((markdown, converter) => markdown.replace(converter.regex, converter.replace), markdownText);

function initializeTitleText() {
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

	popup.getMarkdown(function (markdownFromAddon) {
		markdownElement.innerHTML = getMarkdownAsHTML(markdownFromAddon);

		initializeTitleText();
		initializeActionText();

		setupCloseButton();
		setupActionButton();
	});
});