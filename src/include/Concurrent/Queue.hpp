#ifndef __CONCURRENT_QUEUE_HPP__
#define __CONCURRENT_QUEUE_HPP__

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace Concurrent {

template <typename T>
class Queue {
public:
	bool empty() const {
		std::unique_lock<std::mutex> mlock(mutex_);
		bool isEmpty = queue_.empty();
		mlock.unlock();
		return isEmpty;
	}

	T pop() {
		std::unique_lock<std::mutex> mlock(mutex_);
		while (queue_.empty()) {
			cond_.wait(mlock);
		}
		auto item = std::move(queue_.front());  // added && for move semantics
												//		auto item = queue_.front();;
		queue_.pop();
		return std::move(item);  // also changed to have move()
								 //		return item;
	}

	void pop(T& item) {
		std::unique_lock<std::mutex> mlock(mutex_);
		while (queue_.empty()) {
			cond_.wait(mlock);
		}
		item = queue_.front();
		queue_.pop();
	}

	void push(const T& item) {
		std::unique_lock<std::mutex> mlock(mutex_);
		queue_.push(item);
		mlock.unlock();
		cond_.notify_one();
	}

	void push(T&& item) {
		std::unique_lock<std::mutex> mlock(mutex_);
		queue_.push(std::move(item));
		mlock.unlock();
		cond_.notify_one();
	}

	template <typename... args_T>
	void emplace(args_T&&... args) {
		std::unique_lock<std::mutex> mlock(mutex_);
		queue_.emplace(std::forward<args_T>(args)...);
		mlock.unlock();
		cond_.notify_one();
	}

private:
	std::queue<T> queue_;
	mutable std::mutex mutex_;
	std::condition_variable cond_;
};  // end class ConcurrentQueue

}  // end namespace Concurrent

#endif
