import { useEffect, useState } from "preact/hooks";
import { gellySync } from "../gelly-sync.ts";

export default function useSettingValue(key: string) {
	const [settingValue, setSettingValue] = useState(0);
	const [syncWithChanges, setSyncWithChanges] = useState(false);

	const updateSettingValueWithNewValue = () => {
		gelly.getSettingAsFloat(key, (value) => {
			setSettingValue(value);
			setSyncWithChanges(true);
		});
	};

	useEffect(() => {
		if (!syncWithChanges) {
			return;
		}

		gelly.setSettingAsFloat(key, settingValue);
	}, [settingValue, syncWithChanges]);

	useEffect(updateSettingValueWithNewValue, []);

	// Unfortunately, we need to explicitly wait for when the module tells us to reload the setting values.
	useEffect(() => {
		gellySync.addListener(updateSettingValueWithNewValue);
		return () => gellySync.removeListener(updateSettingValueWithNewValue);
	}, []);

	return [settingValue, setSettingValue] as const;
}
