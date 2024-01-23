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
 */
template <typename Key, typename Value>
class HashTable {
	using HashFunction = uint32_t (*)(const Key &);
	static_assert(detail::IsTypePOD<Value>(), "Value must be a POD type");

private:
	Value *m_memory;
	uint32_t m_capacity;
	uint32_t m_size;

	HashFunction m_hashFunction;

	// in the future we might want to look into load factors
public:
	explicit HashTable(uint32_t startingCapacity, HashFunction hashFunction);
	~HashTable();

	void Insert(const Key &key, const Value &value);
	void Insert(Key &&key, Value &&value);

	// only expose if value is numeric
	template <typename T = Value>
	std::enable_if_t<std::is_arithmetic_v<T>, void> Increment(
		const Key &key, const Value &value
	) {
		const auto hash = m_hashFunction(key);
		const auto index = hash % m_capacity;

		m_memory[index] += value;
	}

	const Value *Find(const Key &key) const;
	const Value *Find(Key &&key) const;

	void Clear();
};

template <typename Key, typename Value>
HashTable<Key, Value>::HashTable(
	uint32_t startingCapacity, HashFunction hashFunction
)
	: m_capacity(startingCapacity), m_size(0), m_hashFunction(hashFunction) {
	m_memory = new Value[m_capacity];
	std::memset(m_memory, 0, sizeof(Value) * m_capacity);
}

template <typename Key, typename Value>
HashTable<Key, Value>::~HashTable() {
	delete[] m_memory;
}

template <typename Key, typename Value>
void HashTable<Key, Value>::Insert(const Key &key, const Value &value) {
	if (m_size >= m_capacity) {
		// nothing inherently wrong with this, but we should probably resize
		// later
	}

	const auto hash = m_hashFunction(key);
	const auto index = hash % m_capacity;

	m_memory[index] = value;
	m_size++;
}

template <typename Key, typename Value>
void HashTable<Key, Value>::Insert(Key &&key, Value &&value) {
	if (m_size >= m_capacity) {
		// nothing inherently wrong with this, but we should probably resize
		// later
	}

	const auto hash = m_hashFunction(key);
	const auto index = hash % m_capacity;

	m_memory[index] = value;
	m_size++;
}

template <typename Key, typename Value>
const Value *HashTable<Key, Value>::Find(const Key &key) const {
	const auto hash = m_hashFunction(key);
	const auto index = hash % m_capacity;

	return &m_memory[index];
}

template <typename Key, typename Value>
const Value *HashTable<Key, Value>::Find(Key &&key) const {
	const auto hash = m_hashFunction(key);
	const auto index = hash % m_capacity;

	return &m_memory[index];
}

template <typename Key, typename Value>
void HashTable<Key, Value>::Clear() {
	m_size = 0;
	std::memset(m_memory, 0, sizeof(Value) * m_capacity);
}

}  // namespace gcr::structs
#endif	// HASHTABLE_H
