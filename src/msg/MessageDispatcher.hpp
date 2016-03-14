

#ifndef __MESSAGEDISPATCHER_HPP__
#define __MESSAGEDISPATCHER_HPP__

#include "msg/MessageExtractor.hpp"
#include "msg/MessageHandler.hpp"
#include <vector>
#include "optional.hpp"
#include "msg/pbmsg.hpp"


namespace msg {
using Utility::optional;



template <typename outerMsg_T, typename msgEnum_T, typename abstractMsg_T,
		  typename... args_T>
class MessageDispatcher {
public:
	using abstractMsg_type = abstractMsg_T;
	using this_type =
		MessageDispatcher<outerMsg_T, msgEnum_T, abstractMsg_T, args_T...>;
	using msgEnum_type = msgEnum_T;
	using outerMsg_type = outerMsg_T;
	using extractorBase_type = MessageExtractorBase<outerMsg_type>;
	using handlerBase_type = MessageHandlerBase<abstractMsg_type, args_T...>;

public:
	// public interface

	template <typename parentInstance_T>
	auto getRegisterFunction(parentInstance_T* parent) {
		return RegisterHelper<parentInstance_T>(this, parent);
	}

	template <typename... handlerArgs_T>
	void handle(const msgEnum_type msgType, const outerMsg_T* msg,
				handlerArgs_T&&... args) {
		const auto entry = findEntry(msgType);
		if (!entry) {
			return;
		}

		(*(entry->handler))(((*(entry->extractor))(msg)),
							std::forward<handlerArgs_T>(args)...);
	}

private:
	// internal data structures
	struct DispatchEntry {
		DispatchEntry(const msgEnum_type& t,
					  std::unique_ptr<extractorBase_type> ex,
					  std::unique_ptr<handlerBase_type> h)
			: type(t), extractor(std::move(ex)), handler(std::move(h)) {}
		const msgEnum_type type;
		std::unique_ptr<extractorBase_type> extractor;
		std::unique_ptr<handlerBase_type> handler;
	};  // end struct DisptachEntry
public:
	template <typename parentInstance_T>
	struct RegisterHelper {
		this_type* m_dispatcher;
		parentInstance_T* m_parent;
		RegisterHelper(this_type* dispatcher, parentInstance_T* p)
			: m_dispatcher(dispatcher), m_parent(p) {}
		template <typename innerMsg_T>
		void entry(
			const msgEnum_type e,
			std::function<const innerMsg_T&(const outerMsg_type*)> extractor,
			std::function<void(parentInstance_T*, const innerMsg_T*, args_T...)>
				handler) {
			auto parent = m_parent;
			return m_dispatcher->registerHandler<innerMsg_T>(
				e, extractor,
				[parent, handler](const innerMsg_T* msg, auto... handlerArgs) {
					return handler(parent, msg, handlerArgs...);
				});
		}  // end function entry
	};	 // end struct RegisterHelper

private:
	// internal member functions

	template <typename innerMsg_T>
	inline void registerHandler(
		const msgEnum_type msgType,
		std::function<const innerMsg_T&(const outerMsg_type*)> extractorFunc,
		std::function<void(const innerMsg_T*, args_T...)> handlerFunc) {
		using extractor_type = MessageExtractor<outerMsg_type, innerMsg_T>;
		using handler_type =
			MessageHandler<abstractMsg_type, innerMsg_T, args_T...>;

		m_dispatchTable.emplace_back(
			msgType, std::make_unique<extractor_type>([=](const auto* msg) {
				return &(extractorFunc(msg));
			}),
			std::make_unique<handler_type>(handlerFunc));
	}

	const DispatchEntry* findEntry(const msgEnum_type msgType) {
		const auto e =
			find_if(m_dispatchTable.cbegin(), m_dispatchTable.cend(),
					[=](const auto& entry) { return entry.type == msgType; });

		if (e == m_dispatchTable.cend()) {
			return nullptr;
		}
		return &(*e);
	}

private:
	// member data
	std::vector<DispatchEntry> m_dispatchTable;

};  // end class MessageDispatcher

}  // end namespace msg

#endif
