

#ifndef __MODULE_HPP__
#define __MODULE_HPP__

#include "msg/MessageDispatcher.hpp"

template <typename messageDispatcher_T>
class Module {
public:
	using Module_type = Module<messageDispatcher_T>;
	using MessageDispatcher_type = messageDispatcher_T;

public:
	Module() = default;
	Module(const Module&) = delete;
	Module(Module&&) = delete;
	virtual ~Module() {}

	void bindHandlers(MessageDispatcher_type* dispatcher) {
		this->bindHandlersImp(dispatcher);
	}

private:
	virtual void bindHandlersImp(MessageDispatcher_type*) = 0;
};  // end class Module

#endif
