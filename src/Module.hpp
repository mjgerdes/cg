

#ifndef __MODULE_HPP__
#define __MODULE_HPP__

#include <functional>
#include "msg/MessageDispatcher.hpp"

template <typename messageDispatcher_T, typename sendFunc_T,
		  typename messageFactoryFunc_T>
class Module {
public:
	using Module_type = Module<messageDispatcher_T, sendFunc_T, messageFactoryFunc_T>;
	using MessageDispatcher_type = messageDispatcher_T;
	using SendFunction_type = sendFunc_T;
	using MessageFactoryFunction_type = messageFactoryFunc_T;

public:
	Module() = default;
	Module(const Module&) = delete;
	Module(Module&&) = delete;
	virtual ~Module() {}

	inline void bindServerMessageFactoryFunction(MessageFactoryFunction_type f) {
		makeServerMessage = f;
	}
	inline void bindSendFunction(SendFunction_type f) { sendMessage = f; }

	void bindHandlers(MessageDispatcher_type* dispatcher) {
		this->bindHandlersImp(dispatcher);
	}

protected:
	SendFunction_type sendMessage;
	MessageFactoryFunction_type makeServerMessage;

private:
	virtual void bindHandlersImp(MessageDispatcher_type*) = 0;
};  // end class Module

#endif
