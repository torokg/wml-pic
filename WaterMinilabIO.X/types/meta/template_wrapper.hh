#ifndef TYPES_META_TEMPLATE_WRAPPER_HH
# define TYPES_META_TEMPLATE_WRAPPER_HH

namespace types::meta
{

template<template<typename...> typename Twd>
struct template_wrapper : metaitem
{
	template<typename... Ts>
	using type = Twd<Ts...>;
};

}
#endif
