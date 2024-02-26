import SettingsRepository, { GraphicsQuality } from "./SettingsRepository.ts";

describe("SettingsRepository", () => {
	const testSettings = {
		graphicsQuality: GraphicsQuality.Low,
		simIterations: 5,
		simSubsteps: 10,
		addonDefaultMod: "blood",
	};

	const localStorageGetItemSpy = jest.spyOn(Storage.prototype, "getItem");
	const localStorageSetItemSpy = jest.spyOn(Storage.prototype, "setItem");

	it("should save the given settings object", () => {
		const repository = new SettingsRepository();

		repository.saveSettings(testSettings);

		const savedSettings = repository.getCurrentSettings();
		expect(savedSettings).toEqual(testSettings);
	});

	it("should return a copy of the saved settings", () => {
		const repository = new SettingsRepository();

		repository.saveSettings(testSettings);
		repository.getCurrentSettings().graphicsQuality = GraphicsQuality.Epic;

		expect(repository.getCurrentSettings().graphicsQuality).not.toEqual(
			GraphicsQuality.Epic,
		);
	});

	it("should save the settings to local storage", () => {
		const repository = SettingsRepository.loadSettingsFromStorage();

		repository.saveSettings(testSettings);

		expect(localStorageSetItemSpy).toBeCalled();
	});

	it("should load the settings from local storage", () => {
		localStorageGetItemSpy.mockReturnValue(JSON.stringify(testSettings));

		const saveRepository = SettingsRepository.loadSettingsFromStorage();
		saveRepository.saveSettings(testSettings);

		const loadRepository = SettingsRepository.loadSettingsFromStorage();
		expect(loadRepository.getCurrentSettings()).toEqual(testSettings);
		expect(localStorageGetItemSpy).toBeCalled();
	});
});
