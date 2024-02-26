import ReversibleSettings from "../utils/ReversibleSettings.ts";
import { Settings } from "../repositories/SettingsRepository.ts";
import { useEffect, useState } from "react";

/**
 * Provides a hook to use a setting from a ReversibleSettings instance.
 * @param reversibleSettings
 * @param setting
 * @returns A tuple with the current value of the setting, a function to set the value and a boolean indicating if the setting has been modified.
 */
export default function useSetting<K extends keyof Settings>(
	reversibleSettings: ReversibleSettings,
	setting: K,
): [Settings[K], (value: Settings[K]) => void, boolean] {
	const [value, setValue] = useState(
		reversibleSettings.getCurrentSettings()[setting],
	);

	const [isModified, setIsModified] = useState(
		reversibleSettings.isSettingModified(setting),
	);

	useEffect(() => {
		const settings = reversibleSettings.getCurrentSettings();
		settings[setting] = value;
		reversibleSettings.setCurrentSettings(settings);
	}, [value, reversibleSettings, setting]);

	useEffect(() => {
		const revertSubscriber = () => {
			setValue(reversibleSettings.getCurrentSettings()[setting]);
		};

		reversibleSettings.registerRevertSubscriber(revertSubscriber);

		return () => {
			reversibleSettings.unregisterRevertSubscriber(revertSubscriber);
		};
	}, [reversibleSettings, setting]);

	useEffect(() => {
		setIsModified(reversibleSettings.isSettingModified(setting));

		const saveSubscriber = () => {
			setIsModified(false);
		};

		reversibleSettings.registerSaveSubscriber(saveSubscriber);

		return () => {
			reversibleSettings.unregisterSaveSubscriber(saveSubscriber);
		};
	}, [reversibleSettings, setting, value]);

	return [value, setValue, isModified];
}
