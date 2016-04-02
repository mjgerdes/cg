

#ifndef __HULL_HPP__
#define __HULL_HPP__

#include <vector>
#include "HullData.pb.h"
#include "System.hpp"


class Hull {
public:
	using Id_type = data::HullData::HullId;
	using data_type = data::HullData;

	struct HullVerifier {
		bool operator()(data_type* hull);
	};  // end struct HullVerifier
public:
	Hull(const data_type*);

	Hull() = delete;
	Hull(const Hull&) = default;
	Hull(Hull&& other) = default;
	Hull& operator=(const Hull&) = default;
	Hull& operator=(Hull&& other) = default;

public:
	inline Id_type id() const { return m_data->id(); }
	inline unsigned int size() const { return m_systems.size(); }
	inline const std::vector<System>& systems() const { return m_systems; }
	bool trySetSystemsFromSequence(const std::vector<System>&);
	bool trySetSystem(System::System_ptr system, size_t pos);
	bool trySetSystem(const System& system, size_t position);
	bool verifySystemForPos(const System&, size_t) const;

private:
	const data_type* m_data;
	std::vector<System> m_systems;
};  // end class Hull

#endif
