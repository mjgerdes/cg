

#ifndef __MESSAGEXTRACTOR_HPP__
#define __MESSAGEXTRACTOR_HPP__

#include "msg/pbmsg.hpp"

namespace msg {
	template<typename outerMsg_T>
class MessageExtractorBase {
public:
		using outerMsg_type = outerMsg_T;
	virtual ~MessageExtractorBase() {}
	virtual const pbmsg_type* operator()(const outerMsg_type*) = 0;
};  // end class MessageExtractorBase

	template <typename outerMsg_T, typename innerMsg_T>
	class MessageExtractor : public MessageExtractorBase<outerMsg_T> {
public:
		using innerMsg_type = innerMsg_T;
		using outerMsg_type = outerMsg_T;
		using function_type = std::function<const innerMsg_type*(const outerMsg_type*)>;

		MessageExtractor(function_type f) : m_getMessage(f) {}
	virtual const pbmsg_type* operator()(const outerMsg_type* msg) override {
		return dynamic_cast<const pbmsg_type*>(m_getMessage(msg));
	}
private:
		function_type m_getMessage;
};  // end class MessageExtractor

} // end namespace msg


#endif
