export default function deepEqual(a: unknown, b: unknown): boolean {
	if (someAreNotDictionaries(a, b)) {
		return a === b;
	}

	const aAsDictionary = a as Record<string, unknown>;
	const bAsDictionary = b as Record<string, unknown>;

	if (notSameShape(aAsDictionary, bAsDictionary)) {
		return false;
	}

	return Object.keys(aAsDictionary).every((key) =>
		deepEqual(aAsDictionary[key], bAsDictionary[key]),
	);
}

export function notSameShape(
	a: Record<string, unknown>,
	b: Record<string, unknown>,
): boolean {
	return Object.keys(a).length !== Object.keys(b).length;
}

export function bothAreDictionaries(
	a: unknown,
	b: unknown,
): a is Record<string, unknown> {
	return isDictionary(a) && isDictionary(b);
}

function isDictionary(a: unknown): a is Record<string, unknown> {
	// https://stackoverflow.com/a/71975382
	// As we're only interested in objects, this check is sufficient.
	return a instanceof Object && a.constructor === Object;
}

function someAreNotDictionaries(
	a: unknown,
	b: unknown,
): a is Record<string, unknown> {
	return !bothAreDictionaries(a, b);
}
