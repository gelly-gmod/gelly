import deepEqual, { notSameShape, bothAreDictionaries } from "./deep-equal.ts";

describe("deepEqual", () => {
	it("should return true if the dictionaries are equal", () => {
		const a = { a: { foo: 20, bar: "hi!" }, b: { qux: 10 } };
		const b = { a: { foo: 20, bar: "hi!" }, b: { qux: 10 } };

		expect(deepEqual(a, b)).toBe(true);
	});

	it("should return false if the dictionaries are not equal", () => {
		const a = { a: { foo: 20, bar: "hi!" }, b: { qux: 10 } };
		const b = { a: { foo: 12, bar: "hi!" }, b: { qux: 11, superQux: 120 } };

		expect(deepEqual(a, b)).toBe(false);
	});
});

describe("notSameShape", () => {
	it("should return true if the dictionaries are not the same shape", () => {
		const a = { a: { foo: 20, bar: "hi!" }, b: { qux: 10 } };
		const b = { a: { foo: 20, bar: "hi!" } };

		expect(notSameShape(a, b)).toBe(true);
	});

	it("should return false if the dictionaries are the same shape", () => {
		const a = { a: { foo: 20, bar: "hi!" }, b: { qux: 10 } };
		const b = { a: { foo: 20, bar: "hi!" }, b: { qux: 11 } };

		expect(notSameShape(a, b)).toBe(false);
	});
});

describe("bothAreDictionaries", () => {
	it("should return true if both arguments are dictionaries", () => {
		const a = { foo: 1337, quux: "hi!" };
		const b = { bar: 42, baz: "hello!" };

		expect(bothAreDictionaries(a, b)).toBe(true);
	});

	it("should return false if both arguments are not dictionaries", () => {
		const a = "foo";
		const b = "bar";

		expect(bothAreDictionaries(a, b)).toBe(false);
	});

	it("should return false if one argument is not a dictionary", () => {
		const a = { foo: 1337, quux: "hi!" };
		const b = "bar";

		expect(bothAreDictionaries(a, b)).toBe(false);
	});
});
