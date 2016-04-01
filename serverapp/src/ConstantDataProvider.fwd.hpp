

#ifndef __CONSTANTDATAPROVIDER_FWD_HPP__
#define __CONSTANTDATAPROVIDER_FWD_HPP__


template <typename data_T>
struct StandardVerifier; 

template <typename data_T, typename wrapper_T, typename loader_T,
		  typename verifier_T = StandardVerifier<data_T>>
	class ConstantDataProvider;

#endif
