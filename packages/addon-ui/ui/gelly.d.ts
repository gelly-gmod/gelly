declare namespace gelly {
	declare type Preset = {
		name: string;
		color: string;
	};

	declare type Mod = {
		name: string;
		id: string;
		enabled: boolean;
	};

	/**
	 * Fetches a setting key from the Gelly settings
	 * @param key The key to fetch
	 * @param callback The callback to call with the setting value
	 * @returns The setting value as a float
	 */
	function getSettingAsFloat(
		key: string,
		callback: (value: number) => void,
	): void;

	/**
	 * Fetches a setting key from the Gelly settings
	 * @param key The key to fetch
	 * @param callback The callback to call with the setting value
	 * @returns The setting value as a string
	 */
	function getSettingAsString(
		key: string,
		callback: (value: string) => void,
	): void;

	/**
	 * Writes to a setting key in the Gelly settings
	 * @param key The key to write to
	 * @param value The value to write
	 * @returns void
	 */
	function setSettingAsFloat(key: string, value: number): void;

	/**
	 * Writes to a setting key in the Gelly settings
	 * @param key The key to write to
	 * @param value The value to write
	 * @returns void
	 */
	function setSettingAsString(key: string, value: string): void;

	function resetSetting(key: string): void;

	function getActiveParticles(callback: (count: number) => void): void;
	function getVersion(callback: (version: string) => void): void;
	function getPresets(callback: (presets: Preset[]) => void): void;
	function getMods(callback: (mods: Mod[]) => void): void;
	function hide(): void;
	function getMenuBindKey(callback: (key: string) => void): void;
	function selectPreset(preset: string): void;
	function selectMod(modId: string): void;
}
