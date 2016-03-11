

#ifndef __MESSAGEHANDLER_HPP__
#define __MESSAGEHANDLER_HPP__

#include "pbmsg.hpp"

namespace msg {

	template <typename abstractMsg_T, typename... args_T>
class MessageHandlerBase {
public:
	virtual ~MessageHandlerBase() {}

	virtual void operator()(const abstractMsg_T*, args_T...) = 0;
};  // end class MessageHandlerBase

	template <typename abstractMsg_T, typename concreteMsg_T, typename... args_T> 
class MessageHandler : public MessageHandlerBase<abstractMsg_T, args_T...> {
public:
	using abstractMsg_type = abstractMsg_T;
	using concreteMsg_type = concreteMsg_T;
	using handler_type = std::function<void(const concreteMsg_type*, args_T...)>;
	MessageHandler(handler_type handler) : m_f(handler) {}

	virtual void operator()(const abstractMsg_type* msg, args_T... args) override {
		m_f(static_cast<const concreteMsg_type*>(msg), std::forward<args_T>(args)...);
	}

private:
	handler_type m_f;
};  // end class MessageHandler

}  // end namespace msg

#endif
