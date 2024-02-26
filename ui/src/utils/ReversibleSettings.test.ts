import SettingsRepository, {
	GraphicsQuality,
} from "../repositories/SettingsRepository.ts";
import ReversibleSettings from "./ReversibleSettings.ts";
import structuredClone from "@ungap/structured-clone";

function loadReversibleSettings(): ReversibleSettings {
	const repository = SettingsRepository.loadSettingsFromStorage();
	return new ReversibleSettings(repository);
}

describe("ReversibleSettings", () => {
	const originalSettings = {
		graphicsQuality: GraphicsQuality.Low,
		simIterations: 5,
		simSubsteps: 10,
		addonDefaultMod: "blood",
	};

	const localStorageGetItemSpy = jest.spyOn(Storage.prototype, "getItem");

	localStorageGetItemSpy.mockReturnValue(JSON.stringify(originalSettings));

	it("should not save the modifications when reverted", () => {
		const reversibleSettings = loadReversibleSettings();
		const newSettings = structuredClone(originalSettings);
		newSettings.graphicsQuality = GraphicsQuality.Epic;

		reversibleSettings.setCurrentSettings(newSettings);
		reversibleSettings.revert();

		expect(reversibleSettings.getCurrentSettings()).toEqual(
			originalSettings,
		);
	});

	it("should save the settings when calling saveSettings", () => {
		const reversibleSettings = loadReversibleSettings();
		const newSettings = structuredClone(originalSettings);
		newSettings.graphicsQuality = GraphicsQuality.Epic;

		reversibleSettings.setCurrentSettings(newSettings);
		reversibleSettings.saveSettings();

		expect(reversibleSettings.getCurrentSettings()).toEqual(newSettings);
	});

	it("should return the same settings when reverting just after saving", () => {
		const reversibleSettings = loadReversibleSettings();

		const newSettings = structuredClone(originalSettings);
		newSettings.graphicsQuality = GraphicsQuality.Epic;

		reversibleSettings.setCurrentSettings(newSettings);
		reversibleSettings.saveSettings();
		reversibleSettings.revert();

		expect(reversibleSettings.getCurrentSettings()).toEqual(newSettings);
	});

	const revertSettings = (reversibleSettings: ReversibleSettings) => {
		const newSettings = structuredClone(originalSettings);
		newSettings.graphicsQuality = GraphicsQuality.Epic;

		reversibleSettings.setCurrentSettings(newSettings);
		reversibleSettings.revert();
	};

	it("should notify subscribers when settings are reverted", () => {
		const subscriber = jest.fn();
		const reversibleSettings = loadReversibleSettings();
		reversibleSettings.registerRevertSubscriber(subscriber);

		revertSettings(reversibleSettings);

		expect(subscriber).toHaveBeenCalledTimes(1);
	});

	it("should properly unregister a revert subscriber", () => {
		const subscriber = jest.fn();
		const reversibleSettings = loadReversibleSettings();
		const listener =
			reversibleSettings.registerRevertSubscriber(subscriber);

		reversibleSettings.unregisterRevertSubscriber(listener);
		revertSettings(reversibleSettings);

		expect(subscriber).not.toHaveBeenCalled();
	});

	it("should throw an error when unregistering a non-existent revert subscriber", () => {
		const reversibleSettings = loadReversibleSettings();
		const subscriber = () => {};

		expect(() =>
			reversibleSettings.unregisterRevertSubscriber(subscriber),
		).toThrowError("Subscriber not found");
	});

	it("should notify subscribers when settings are saved", () => {
		const subscriber = jest.fn();
		const reversibleSettings = loadReversibleSettings();
		reversibleSettings.registerSaveSubscriber(subscriber);

		reversibleSettings.saveSettings();

		expect(subscriber).toHaveBeenCalledTimes(1);
	});

	it("should properly unregister a save subscriber", () => {
		const subscriber = jest.fn();
		const reversibleSettings = loadReversibleSettings();
		const listener = reversibleSettings.registerSaveSubscriber(subscriber);

		reversibleSettings.unregisterSaveSubscriber(listener);
		reversibleSettings.saveSettings();

		expect(subscriber).not.toHaveBeenCalled();
	});

	it("should throw an error when unregistering a non-existent save subscriber", () => {
		const reversibleSettings = loadReversibleSettings();
		const subscriber = () => {};

		expect(() =>
			reversibleSettings.unregisterSaveSubscriber(subscriber),
		).toThrowError("Subscriber not found");
	});

	describe("isSettingModified", () => {
		it("should return false for a setting that has not been modified", () => {
			const reversibleSettings = loadReversibleSettings();
			const newSettings = structuredClone(originalSettings);
			newSettings.graphicsQuality = GraphicsQuality.Epic;

			reversibleSettings.setCurrentSettings(newSettings);

			expect(reversibleSettings.isSettingModified("simIterations")).toBe(
				false,
			);
		});

		it("should return true for a setting that has been modified", () => {
			const reversibleSettings = loadReversibleSettings();
			const newSettings = structuredClone(originalSettings);
			newSettings.graphicsQuality = GraphicsQuality.Epic;

			reversibleSettings.setCurrentSettings(newSettings);

			expect(
				reversibleSettings.isSettingModified("graphicsQuality"),
			).toBe(true);
		});
	});
});
