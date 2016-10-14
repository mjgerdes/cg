

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
		std::vector<short> u = m_range;
		shuffle(&u);
return [v = std::move(u)]() mutable {
			auto i = v.back();
			v.pop_back();
			return i;
		};
	}

	~RandomProvider();

private:
	std::random_device m_rd;
	std::mt19937 m_mt;
	const std::vector<short> m_range;
};  // end RandomProvider

#endif
