#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <cstdint>
#include <stdexcept>
#include <type_traits>

namespace gcr::structs {
namespace detail {
template <typename T>
constexpr bool IsTypePOD() {
	return std::is_trivially_copyable<T>::value &&
		   std::is_trivially_destructible<T>::value &&
		   std::is_default_constructible<T>::value;
}
}  // namespace detail

/**
 * \brief A simple hash table implementation, meant for numeric values.
 * \tparam Key Type for the key, must be hashable with HashFunction
 * \tparam Value Type for the value, must be POD and addable with itself for
 * fast increment
 * \tparam HashFunction Type for the hash function, must be
 * callable with Key and return uint32_t
 */
template <typename Key, typename Value, typename HashFunction>
class HashTable {
	static_assert(detail::IsTypePOD<Value>(), "Value must be a POD type");
	static_assert(
		std::is_default_constructible<HashFunction>::value,
		"HashFunction must be default constructible"
	);

	static_assert(
		std::is_invocable_r_v<uint32_t, HashFunction, Key>,
		"HashFunction must be callable with Key"
	);

	// also ensure we can add any Value to another Value
	// TODO: Hopefully this is not too restrictive, but incrementing can be
	// faster if we ensure this at compile time
	static_assert(
		std::is_invocable_r_v<Value, decltype(&Value::operator+), Value, Value>,
		"Value must be addable with itself"
	);

private:
	Value *m_memory;
	uint32_t m_capacity;
	uint32_t m_size;

	HashFunction m_hashFunction;

	// in the future we might want to look into load factors
public:
	explicit HashTable(uint32_t startingCapacity);
	~HashTable();

	void Insert(const Key &key, const Value &value);
	void Insert(Key &&key, Value &&value);

	void Increment(const Key &key, const Value &value);

	const Value *Find(const Key &key) const;
	const Value *Find(Key &&key) const;

	void Clear();
};

template <typename Key, typename Value, typename HashFunction>
HashTable<Key, Value, HashFunction>::HashTable(uint32_t startingCapacity)
	: m_capacity(startingCapacity), m_size(0) {
	m_memory = new Value[m_capacity];
	std::memset(m_memory, 0, sizeof(Value) * m_capacity);
}

template <typename Key, typename Value, typename HashFunction>
HashTable<Key, Value, HashFunction>::~HashTable() {
	delete[] m_memory;
}

template <typename Key, typename Value, typename HashFunction>
void HashTable<Key, Value, HashFunction>::Insert(
	const Key &key, const Value &value
) {
	if (m_size >= m_capacity) {
		throw std::runtime_error("Hash table is full");
	}

	const auto hash = m_hashFunction(key);
	const auto index = hash % m_capacity;

	m_memory[index] = value;
	m_size++;
}

template <typename Key, typename Value, typename HashFunction>
void HashTable<Key, Value, HashFunction>::Insert(Key &&key, Value &&value) {
	if (m_size >= m_capacity) {
		throw std::runtime_error("Hash table is full");
	}

	const auto hash = m_hashFunction(key);
	const auto index = hash % m_capacity;

	m_memory[index] = value;
	m_size++;
}

template <typename Key, typename Value, typename HashFunction>
void HashTable<Key, Value, HashFunction>::Increment(
	const Key &key, const Value &value
) {
	if (m_size >= m_capacity) {
		throw std::runtime_error("Hash table is full");
	}

	const auto hash = m_hashFunction(key);
	const auto index = hash % m_capacity;

	m_memory[index] = m_memory[index] + value;
}

template <typename Key, typename Value, typename HashFunction>
const Value *HashTable<Key, Value, HashFunction>::Find(const Key &key) const {
	const auto hash = m_hashFunction(key);
	const auto index = hash % m_capacity;

	return &m_memory[index];
}

template <typename Key, typename Value, typename HashFunction>
const Value *HashTable<Key, Value, HashFunction>::Find(Key &&key) const {
	const auto hash = m_hashFunction(key);
	const auto index = hash % m_capacity;

	return &m_memory[index];
}

template <typename Key, typename Value, typename HashFunction>
void HashTable<Key, Value, HashFunction>::Clear() {
	m_size = 0;
	std::memset(m_memory, 0, sizeof(Value) * m_capacity);
}

}  // namespace gcr::structs
#endif	// HASHTABLE_H
