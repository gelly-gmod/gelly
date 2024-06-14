function onSettingChanged(inputElement) {
	let value = inputElement.value;
	if (inputElement.type === 'checkbox') {
		value = inputElement.checked;
	}

	gelly.setSetting(inputElement.id, value);
}

function registerControl(inputElement) {
	inputElement.addEventListener('change', function () {
		onSettingChanged(inputElement);
	});
}

function registerControls() {
	let inputs = document.getElementsByTagName('input');
	for (let i = 0; i < inputs.length; i++) {
		registerControl(inputs[i]);
	}
}

function registerSaveButton() {
	let saveButton = document.getElementById('preset-save');
	saveButton.addEventListener('click', function () {
		gelly.savePreset();
	});
}

function registerTestButton() {
	let testButton = document.getElementById('preset-test');
	testButton.addEventListener('click', function () {
		gelly.testPreset();
	});
}

function init() {
	console.log("GPC: Registering controls...")
	registerControls();
	registerSaveButton();
	registerTestButton();
	console.log("GPC: Controls registered.")
}

document.addEventListener('DOMContentLoaded', init);