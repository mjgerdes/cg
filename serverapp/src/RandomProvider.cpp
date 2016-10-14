

#include "RandomProvider.hpp"
#include <algorithm>


std::vector<short> initRange() {
	std::vector<short> v;
	for(short i=0; i < 256; ++i) {
		v.push_back(i);
	}
	return v;
}

RandomProvider::RandomProvider() : m_rd(), m_mt(m_rd()), m_range(initRange()) {}


RandomProvider::~RandomProvider() {}
