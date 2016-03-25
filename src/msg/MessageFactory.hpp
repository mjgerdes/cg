

#ifndef __MESSAGEFACTORY_HPP__
#define __MESSAGEFACTORY_HPP__
#include <iostream>
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
		RecycleMessage(const RecycleMessage&) = delete;
		//		RecycleMessage& operator=(const RecycleMessage&) = delete;
		//				const RecycleMessage& operator=(const RecycleMessage&) =
		// delete;
		//		void operator=(RecycleMessage&&) = delete;
		//				void operator=(const RecycleMessage&&) = delete;
		RecycleMessage(RecycleMessage&& other)
			: m_owner(other.m_owner),
			  m_index(other.releaseIndex()),
			  m_mutex(other.m_mutex),
			  m_msg(other.m_msg) {}
		RecycleMessage(const RecycleMessage&& other)
			: m_owner(other.m_owner),
			  m_index(other.releaseIndex()),
			  m_mutex(other.m_mutex),
			  m_msg(other.m_msg) {}
		~RecycleMessage() {
			if (m_index != ~0) {
				m_owner->recycle(m_index);
			}
		}

		message_type& operator*() const { return m_msg; }

		message_type* operator->() const { return &m_msg; }

		std::mutex& getMutex() const { return *m_mutex; }

		index_type releaseIndex() const {
			auto tmp = m_index;
			m_index = ~0;
			return tmp;
		}

	private:
		RecycleMessage(owner_type* owner, const index_type index,
					   std::mutex* mutex, message_type& msg)
			: m_owner(owner), m_index(index), m_mutex(mutex), m_msg(msg) {
			// new
			m_msg.Clear();
		}

	private:
		owner_type* m_owner;
		mutable index_type m_index;
		mutable std::mutex* m_mutex;
		message_type& m_msg;
	};  // struct RecycleMessage

public:
	// public member functions

	MessageFactory(unsigned int initialSize) : m_isGrowing(false) {
		for (uint n = 0; n < initialSize; ++n) {
			m_freeMessageIndices.push_back(n);
			m_mutexArena.emplace_back(new std::mutex);
		}
		m_arena.resize(initialSize);
	}

	RecycleMessage makeRecycleMessage() {
		const auto i = getFreeIndex();
		return std::move(
			RecycleMessage(this, i, &(*(m_mutexArena[i])), m_arena[i]));
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
		std::unique_lock<std::mutex> indexLock(m_indexMutex);

		m_freeMessageIndices.push_back(i);
		indexLock.unlock();
		m_fullyStockedCond.notify_one();
	}

	void grow() {
		if (m_isGrowing) {
			return;
		}
		m_isGrowing = true;

		std::unique_lock<std::mutex> indexLock(m_indexMutex);

		m_fullyStockedCond.wait(indexLock,
								[=]() { return this->isFullyStocked(); });

		const auto size = m_arena.size();
		const auto newSize = size * 2;

		for (uint n = size; n < newSize; ++n) {
			m_freeMessageIndices.push_back(n);
			m_mutexArena.emplace_back(new std::mutex);
		}
		m_arena.resize(newSize);

		m_isGrowing = false;
	}

	bool isFullyStocked() {
		//		std::lock_guard<std::mutex> indexLock(m_indexMutex);
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
	std::vector<std::unique_ptr<std::mutex>> m_mutexArena;
};  // class MessageFactory} // namespace msg

}  // end namespace msg
#endif
