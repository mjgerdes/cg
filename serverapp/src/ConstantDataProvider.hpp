

#ifndef __CONSTANTDATAPROVIDER_HPP__
#define __CONSTANTDATAPROVIDER_HPP__
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <string>
#include <vector>

template <typename data_T>
struct StandardVerifier {
	bool operator()(data_T* data) { return true; }
};  // end struct StandardVerifier

template <typename data_T, typename wrapper_T, typename loader_T,
		  typename verifier_T = StandardVerifier<data_T>>
class ConstantDataProvider {
public:
	using data_type = data_T;
	using wrapper_type = wrapper_T;
	using loader_type = loader_T;
	using verifier_type = verifier_T;
	using container_type = std::vector<data_type>;

	using id_type = typename wrapper_type::id_type;

public:
	ConstantDataProvider(const std::string& dataPath)
		: m_dataPath(dataPath), m_data(), m_loader(), m_verifier() {
		init();
	}

	ConstantDataProvider() = delete;
	ConstantDataProvider(const ConstantDataProvider&) = delete;
	ConstantDataProvider(ConstantDataProvider&&) = delete;
	void operator=(const ConstantDataProvider&) = delete;

	wrapper_type get(const id_type id) const {
		return std::move(wrapper_type(&(m_data.at(id))));
	}

private:
	void verify() {
		for (int i = 0; i < m_data.size(); ++i) {
			if (i != m_data[i].id()) {
				std::stringstream msg;
				msg << "error in ConstantDataProvider: id " << m_data[i].id()
					<< "does not match index " << i << std::endl;
				throw std::runtime_error(msg.str());
			}
		}  // for
	}

	void init() {
		m_loader(m_dataPath, [this](data_type& e) {
			auto result = this->m_verifier(&e);
			this->m_data.push_back(e);
			return result;
		});

		std::sort(m_data.begin(), m_data.end(),
				  [](const auto& lhs, const auto& rhs) {
					  return lhs.id() < rhs.id();
				  });
		verify();
	}

private:
	std::string m_dataPath;
	container_type m_data;
	loader_type m_loader;
	verifier_type m_verifier;

};  // end class ConstantDataProvider

#endif
