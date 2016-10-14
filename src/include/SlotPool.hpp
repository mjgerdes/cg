

#ifndef __SLOTPOOL_HPP__
#define __SLOTPOOL_HPP__

#include <vector>
// not thread safe
template <typename element_T>
class SlotPool {
public:
	using owner_type = SlotPool<element_T>;
	using element_type = element_T;
	using index_type = int;
	using SP = SlotPool<element_type>;
	struct Slot {
	public:
		friend owner_type;
		Slot() : m_index(owner_type::getInstance()->getFreeIndex()) {}

		Slot(const Slot&) = delete;
		Slot(Slot&& other) : m_index(other.releaseIndex()) {}

		Slot(const Slot&& other) : m_index(other.releaseIndex()) {}

		Slot& operator=(Slot&& other) {
			m_index = other.releaseIndex();
			return *this;
		}

		~Slot() {
			if (m_index != ~0) {
				owner_type::getInstance()->m_freeList.push_back(m_index);
			}
		}

		bool operator==(const index_type i) { return m_index == i; }

		element_type& operator*() const {
			return owner_type::getInstance()->m_arena[m_index];
		}

		element_type* operator->() const {
			return &(owner_type::getInstance()->m_arena[m_index]);
		}

		index_type releaseIndex() const {
			auto tmp = m_index;
			m_index = ~0;
			return tmp;
		}

	private:
		Slot(const index_type index) : m_index(index) {}

	private:
		mutable index_type m_index;
	};  // end Slot

public:
	static SlotPool<element_type>* getInstance() {
		static SlotPool<element_type> instance{32};
		return &instance;
	}

	static index_type create() { return SP::getInstance()->getFreeIndex(); }

	static element_type& at(index_type i) {
		return SP::getInstance()->m_arena[i];
	}

	static void erase(index_type i) {
		SP::getInstance()->m_freeList.push_back(i);
	}

	static int size() { return SP::getInstance()->m_arena.size(); }

private:
	SlotPool(unsigned int initialSize) {
		for (uint n = 0; n < initialSize; ++n) {
			m_freeList.push_back(n);
		}
		m_arena.resize(initialSize);
	}  // end constructor

private:
	index_type getFreeIndex() {
		if (m_freeList.empty()) {
			grow();
			return getFreeIndex();
		}
		const auto i = m_freeList.back();
		m_freeList.pop_back();
		return i;
	}  // end getFreeIndex

	void grow() {
		const auto size = m_arena.size();
		const auto newSize = size * 2;
		for (uint n = size; n < newSize; ++n) {
			m_freeList.push_back(n);
		}
		m_arena.resize(newSize);
	}  // end grow

private:
	// friend class Slot;

	std::vector<element_type> m_arena;
	std::vector<index_type> m_freeList;

};  // end SlotPool

#endif
