#ifndef TYPES_META_TEMPLATE_ARGUMENTS_OF_HH
# define TYPES_META_TEMPLATE_ARGUMENTS_OF_HH
namespace types::meta
{
	template<typename T>
	struct template_arguments_of;

	template<template<typename...> typename Tt, typename... Ts>
	struct template_arguments_of<Tt<Ts...>>
	{
		typedef type_list<Ts...> type;
	};

	template<template<typename...> typename Tt, typename... Ts>
	using template_arguments_of_t = typename template_arguments_of<Tt<Ts...>>::type;
}
#endif
