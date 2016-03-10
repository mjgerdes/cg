

#ifndef __UTILITY_HPP__
#define __UTILITY_HPP__

#include <sstream>

namespace Utility {
	
inline void addToStream(std::ostringstream&) {}

template<typename T, typename... Args>
void addToStream(std::ostringstream& a_stream, T&& a_value, Args&&... a_args)
{
    a_stream << std::forward<T>(a_value);
    addToStream(a_stream, std::forward<Args>(a_args)...);
}

	template <typename... args_T>
	auto concat(args_T&&... args) -> std::string {
		std::ostringstream msgText;
		addToStream(msgText, std::forward<args_T>(args)...);
		return msgText.str();
	}
} // end namespace
#endif
