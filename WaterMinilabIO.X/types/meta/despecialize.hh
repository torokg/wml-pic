#ifndef TYPES_META_DESPECIALIZE_HH
# define TYPES_META_DESPECIALIZE_HH
# include "template_wrapper.hh"
namespace types::meta
{

/*! When supplied a fully specialized class template, defines a member template 
 *  named type which is equivalent to the original class template */
template<typename T>
struct despecialize;

template<template<typename...> typename Tt, typename... Ts>
struct despecialize<Tt<Ts...>> : template_wrapper<Tt> {};

}
#endif
