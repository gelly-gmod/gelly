export type Settings = {
	graphicsQuality: GraphicsQuality;
	simIterations: number;
	simSubsteps: number;
	addonDefaultMod: string;
};

export enum GraphicsQuality {
	Low = "Low",
	Medium = "Medium",
	High = "High",
	Epic = "Epic",
}

const GELLY_SETTINGS_KEY = "gelly-settings";

import structuredClone from "@ungap/structured-clone";

export default class SettingsRepository {
	private settings: Settings;

	/**
	 * Creates a new instance of the SettingsRepository with empty settings.
	 */
	constructor() {
		this.settings = {
			graphicsQuality: GraphicsQuality.Low,
			simIterations: 0,
			simSubsteps: 0,
			addonDefaultMod: "",
		};
	}

	/**
	 * Provides a new instance of the SettingsRepository with the previously saved settings.
	 */
	public static loadSettingsFromStorage(): SettingsRepository {
		const repository = new SettingsRepository();
		repository.loadSettingsFromLocalStorage();

		return repository;
	}

	private loadSettingsFromLocalStorage(): void {
		const savedSettings = localStorage.getItem(GELLY_SETTINGS_KEY);
		if (savedSettings) {
			this.settings = JSON.parse(savedSettings);
		}
	}

	public getCurrentSettings(): Settings {
		return structuredClone(this.settings);
	}

	public saveSettings(settings: Settings): void {
		this.settings = structuredClone(settings);
		this.saveSettingsToLocalStorage();
	}

	private saveSettingsToLocalStorage(): void {
		localStorage.setItem(GELLY_SETTINGS_KEY, JSON.stringify(this.settings));
	}
}
