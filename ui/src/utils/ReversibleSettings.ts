import SettingsRepository, {
	Settings,
} from "../repositories/SettingsRepository.ts";

import structuredClone from "@ungap/structured-clone";

export type SettingsEventSubscriber = () => void;

/**
 * Allows for modifying settings using the SettingsRepository and having the ability to revert the changes before saving them.
 */
export default class ReversibleSettings {
	private currentSettings: Settings;
	private settingRepository: SettingsRepository;
	private readonly revertSubscribers: SettingsEventSubscriber[];
	private readonly saveSubscribers: SettingsEventSubscriber[];

	constructor(settingsRepository: SettingsRepository) {
		this.settingRepository = settingsRepository;
		this.currentSettings = this.settingRepository.getCurrentSettings();
		this.revertSubscribers = [];
		this.saveSubscribers = [];
	}

	revert(): void {
		this.currentSettings = this.settingRepository.getCurrentSettings();
		for (const subscriber of this.revertSubscribers) {
			subscriber();
		}
	}

	registerRevertSubscriber(
		subscriber: SettingsEventSubscriber,
	): SettingsEventSubscriber {
		this.revertSubscribers.push(subscriber);
		return subscriber;
	}

	unregisterRevertSubscriber(subscriber: SettingsEventSubscriber): void {
		const index = this.revertSubscribers.indexOf(subscriber);
		if (index !== -1) {
			this.revertSubscribers.splice(index, 1);
		} else {
			throw new Error("Subscriber not found");
		}
	}

	registerSaveSubscriber(
		subscriber: SettingsEventSubscriber,
	): SettingsEventSubscriber {
		this.saveSubscribers.push(subscriber);
		return subscriber;
	}

	unregisterSaveSubscriber(subscriber: SettingsEventSubscriber): void {
		const index = this.saveSubscribers.indexOf(subscriber);
		if (index !== -1) {
			this.saveSubscribers.splice(index, 1);
		} else {
			throw new Error("Subscriber not found");
		}
	}

	getCurrentSettings(): Settings {
		return structuredClone(this.currentSettings);
	}

	setCurrentSettings(settings: Settings): void {
		this.currentSettings = structuredClone(settings);
	}

	isSettingModified(setting: keyof Settings): boolean {
		return (
			this.currentSettings[setting] !==
			this.settingRepository.getCurrentSettings()[setting]
		);
	}

	saveSettings(): void {
		this.settingRepository.saveSettings(this.currentSettings);
		for (const subscriber of this.saveSubscribers) {
			subscriber();
		}
	}
}
