

#ifndef __MESSAGEFACTORY_HPP__
#define __MESSAGEFACTORY_HPP__

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <vector>

namespace msg {

template <typename message_T>
class MessageFactory {
public:
	using owner_type = MessageFactory<message_T>;
	using message_type = message_T;
	using index_type = unsigned int;

public:
	struct RecycleMessage {
	public:
		friend owner_type;
		~RecycleMessage() { m_owner->recycle(m_index); }

		message_type& operator*() const { return m_msg; }

		message_type* operator->() const { return &m_msg; }

	private:
		RecycleMessage(owner_type* owner, const index_type index,
					   message_type& msg)
			: m_owner(owner), m_index(index),  m_msg(msg) {}

	private:
		owner_type* m_owner;
		const index_type m_index;
		message_type& m_msg;
	};  // struct RecycleMessage

public:
	// public member functions

	MessageFactory(unsigned int initialSize) : m_isGrowing(false) {
		for (uint n = 0; n < initialSize; ++n) {
			m_freeMessageIndices.push_back(n);
		}
		m_arena.resize(initialSize);
	}

	RecycleMessage makeRecycleMessage() {
		const auto i = getFreeIndex();
		return RecycleMessage(this, i, m_arena[i]);
	}

private:
	// internal member functions

	index_type getFreeIndex() {
		while (m_isGrowing) {
//			m_growingCond.wait();
		}

		std::unique_lock<std::mutex> indexLock(m_indexMutex);
		if (m_freeMessageIndices.empty()) {
			indexLock.unlock();
			grow();
			return getFreeIndex();
		}
		const auto i = m_freeMessageIndices.back();
		m_freeMessageIndices.pop_back();
		return i;
	}

	void recycle(const index_type i) {
		std::lock_guard<std::mutex> indexLock(m_indexMutex);
		m_freeMessageIndices.push_back(i);
		m_fullyStockedCond.notify_one();
	}

	void grow() {
		if (m_isGrowing) {
			return;
		}
		m_isGrowing = true;
		std::unique_lock<std::mutex> indexLock(m_indexMutex);
		//		std::unique_lock<std::mutex> arenaLock(m_arenaMutex);

		while (!isFullyStocked()) {
			m_fullyStockedCond.wait(indexLock);
		}
		const auto size = m_arena.size();
		const auto newSize = size * 2;

		for (uint n = size - 1; n < newSize; ++n) {
			m_freeMessageIndices.push_back(n);
		}
		m_arena.resize(newSize);

		m_isGrowing = false;
		//		m_growingCond.notify_all();
	}

	bool isFullyStocked() {
		std::lock_guard<std::mutex> indexLock(m_indexMutex);
		return m_freeMessageIndices.size() == m_arena.size();
	}

private:
	// data members

	std::mutex m_indexMutex;
	std::condition_variable m_growingCond;
	std::condition_variable m_fullyStockedCond;
	std::atomic<bool> m_isGrowing;

	std::vector<index_type> m_freeMessageIndices;
	std::vector<message_type> m_arena;
};  // class MessageFactory} // namespace msg

}  // end namespace msg
#endif
