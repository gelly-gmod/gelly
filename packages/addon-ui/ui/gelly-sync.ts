/* Global namespace such that we can receive synchronization updates from the module. */
export type Preset = {
	name: string;
	color: string;
};

export namespace gellySync {
	// We don't necessarily need any data, but just when to update.
	const listeners: (() => void)[] = [];
	const presetListeners: (() => void)[] = [];
	export const presets: Preset[] = [];

	export function addListener(listener: () => void) {
		listeners.push(listener);
	}

	export function addPresetListener(listener: () => void) {
		presetListeners.push(listener);
	}

	export function removeListener(listener: () => void) {
		const index = listeners.indexOf(listener);
		if (index !== -1) {
			listeners.splice(index, 1);
		}
	}

	export function removePresetListener(listener: () => void) {
		const index = presetListeners.indexOf(listener);
		if (index !== -1) {
			presetListeners.splice(index, 1);
		}
	}

	/**
	 * Notifies all listeners that the synchronization has been updated.
	 * Do not call this function directly, it is called by the module.
	 * @private
	 */
	export function notifyListeners() {
		for (const listener of listeners) {
			listener();
		}
	}

	export function addPreset(preset: Preset) {
		presets.push(preset);
		for (const listener of presetListeners) {
			listener();
		}
	}
}

(<any>window).gellySync = gellySync;
