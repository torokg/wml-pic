#ifndef TYPES_META_CTMAP_HH
# define TYPES_META_CTMAP_HH
# include "pair.hh"
namespace types::meta
{
	
	template<typename... Ts>
	struct ctmap;

	template<
		typename Tkey,
		Tkey tK, typename Tv,
		Tkey... tKs, typename... Tvs
	>
	class ctmap<ctpair<tK,Tv>, ctpair<tKs,Tvs>...>
	{
		template<Tkey key, typename Tx = void>
		struct _get;

		template<Tkey key>
		struct _get<key, std::enable_if_t<key == tK>>
		{ typedef Tv type; };

		template<Tkey key>
		struct _get<key, std::enable_if_t<key != tK>>
		{
			typedef typename
				ctmap<ctpair<tKs, Tvs>...>
				::template _get<key>
				::type type;
		};
		

	public:	
		template<Tkey key>
		using get = typename _get<key>::type;

		template<typename Visitor, typename... Targs>
		static auto visit(Tkey &&key, Targs&&... args)
		{
			if(tK == key)
				return Visitor::template visit<Tv>(std::forward<Targs>(args)...);

			return ctmap<ctpair<tKs,Tvs>...>
				::template visit<Visitor>(
					std::forward<Tkey>(key),
					std::forward<Targs>(args)...
				);
		}
	};

	template<typename Tkey>
	class ctmap<Tkey>
	{
		template<Tkey key>
		struct _get
		{ static_assert(false_swallow_v<std::integral_constant<Tkey,key>>, "no such key in ctmap"); };

		template<Tkey key>
		using get = typename _get<key>::type;

		template<typename Visitor, typename... Targs>
		static auto visit(Tkey &&key, Targs&&... args)
		{ return Visitor::visit(std::forward<Targs>(args)...); }
	};
}
#endif
