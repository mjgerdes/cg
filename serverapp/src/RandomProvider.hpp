

#ifndef __RANDOMPROVIDER_HPP__
#define __RANDOMPROVIDER_HPP__

#include <random>
#include <algorithm>

class RandomProvider {
public:
	RandomProvider();

	template <typename container_T>
	void shuffle(container_T* container) {
std::shuffle(container->begin(), container->end(), this->m_mt);
	}

inline auto getUniqueSequenceGenerator() {
std::vector<short> v = m_range;
shuffle(&v);
return [u = std::move(v)]() mutable { return u.pop_back(); };
}


	~RandomProvider();

private:
	std::random_device m_rd;
	std::mt19937 m_mt;
	const std::vector<short> m_range;
};  // end RandomProvider

#endif
