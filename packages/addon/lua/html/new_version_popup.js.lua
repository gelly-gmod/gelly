const MARKDOWN_TO_HTML_CONVERTERS = [
	{
		regex: /### (.+)/g,
		replace: "<h3>$1</h3>"
	},
	// remove github's line endings in a bullet list
	{
		regex: /- (.+)[\r\n](.+)/g,
		replace: "- $1 $2"
	},
	{
		regex: /- (.+)/g,
		replace: "<p class='bullet'><span class='bullet-point'>&#x2022;</span> $1</p>"
	},
]

const getMarkdownAsHTML =
	markdownText =>
		MARKDOWN_TO_HTML_CONVERTERS.reduce((markdown, converter) => markdown.replace(converter.regex, converter.replace), markdownText);

document.addEventListener('DOMContentLoaded', function () {
	let markdownElement = document.getElementById('content');
	let titleElement = document.getElementById('title');
	let actionElement = document.getElementById('action');
	let closeElement = document.getElementById('close');

	popup.getMarkdown(markdownFromAddon => markdownElement.innerHTML = getMarkdownAsHTML(markdownFromAddon));
	popup.getTitle(title => titleElement.innerHTML = title);
	popup.getAction(action => actionElement.innerHTML = action);

	// We need to create wrapper functions to avoid the element being sent to GMod (and causing errors)
	closeElement.addEventListener('click', () => popup.close());
	actionElement.addEventListener('click', () => popup.performAction());
});