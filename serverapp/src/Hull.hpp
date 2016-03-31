

#ifndef __HULL_HPP__
#define __HULL_HPP__

#include "HullData.pb.h"

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

private:
	const data_type* m_data;
};  // end class Hull

#endif
