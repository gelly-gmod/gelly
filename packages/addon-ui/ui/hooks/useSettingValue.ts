import { useEffect, useState } from "preact/hooks";

export default function useSettingValue(key: string) {
	const [settingValue, setSettingValue] = useState(0);
	const [syncWithChanges, setSyncWithChanges] = useState(false);

	useEffect(() => {
		if (!syncWithChanges) {
			return;
		}

		gelly.setSettingAsFloat(key, settingValue);
	}, [settingValue, syncWithChanges]);

	useEffect(() => {
		gelly.getSettingAsFloat(key, (value) => {
			setSettingValue(value);
			setSyncWithChanges(true);
		});
	}, []);

	return [settingValue, setSettingValue] as const;
}
