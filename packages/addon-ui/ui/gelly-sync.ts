/* Global namespace such that we can receive synchronization updates from the module. */
export namespace gellySync {
	// We don't necessarily need any data, but just when to update.
	const listeners: (() => void)[] = [];

	export function addListener(listener: () => void) {
		listeners.push(listener);
	}

	export function removeListener(listener: () => void) {
		const index = listeners.indexOf(listener);
		if (index !== -1) {
			listeners.splice(index, 1);
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
}

(<any>window).gellySync = gellySync;
