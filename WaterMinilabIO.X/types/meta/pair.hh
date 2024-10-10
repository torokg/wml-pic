#ifndef TYPES_META_PAIR_HH
# define TYPES_META_PAIR_HH
namespace types::meta
{
	/*! Ordered pair composed of two types */
	template<typename Tfirst, typename Tsecond>
	struct ttpair : metaitem
	{
		typedef Tfirst first_type;
		typedef Tsecond second_type;
	};

	/*! Ordered pair composed of a compile-time constant and a type */
	template<auto tFirst, typename Tsecond>
	struct ctpair : metaitem 
	{
		typedef decltype(tFirst) first_type;
		typedef Tsecond second_type;

		constexpr static const first_type first = tFirst;
	};

	/*! Ordered pair composed of a runtime value and a type */
	template<typename Tfirst, typename Tsecond>
	struct vtpair
	{
		typedef Tfirst first_type;
		typedef Tsecond second_type;

		Tfirst first;

		template<typename... Ts>
		inline vtpair(Ts&&... vs)
			: first(std::forward<Ts>(vs)...)
		{}
	};

	/*! Ordered pair composed of a type and a compile-time constant */
	template<typename Tfirst, auto tSecond>
	struct tcpair : metaitem 
	{
		typedef Tfirst first_type;
		typedef decltype(tSecond) second_type;

		constexpr static const second_type second = tSecond;
	};

	/*! Ordered pair composed of two compile-time constants */
	template<auto tFirst, auto tSecond>
	struct ccpair : metaitem 
	{
		typedef decltype(tFirst) first_type;
		typedef decltype(tSecond) second_type;

		constexpr static const first_type first = tFirst;
		constexpr static const second_type second = tSecond;
	};

	/*! Ordered pair composed of a runtime value and a compile-time constant */
	template<typename Tfirst, auto tSecond>
	struct vcpair
	{
		typedef Tfirst first_type;
		typedef decltype(tSecond) second_type;

		Tfirst first;
		constexpr static const second_type second = tSecond;

		template<typename... Ts>
		inline vcpair(Ts&&... vs)
			: first(std::forward<Ts>(vs)...)
		{}
	};

	/*! Ordered pair composed of a type and a runtime value */
	template<typename Tfirst, typename Tsecond>
	struct tvpair
	{
		typedef Tfirst first_type;
		typedef Tsecond second_type;

		Tsecond second;

		template<typename... Ts>
		inline tvpair(Ts&&... vs)
			: second(std::forward<Ts>(vs)...)
		{}
	};

	/*! Ordered pair composed of a compile-time constant and a runtime value */
	template<auto tFirst, typename Tsecond>
	struct cvpair
	{
		typedef decltype(tFirst) first_type;
		typedef Tsecond second_type;

		constexpr static const first_type first = tFirst;
		Tsecond second;

		cvpair() = default;
		cvpair(const cvpair<tFirst,Tsecond>&) = default;
		cvpair(cvpair<tFirst,Tsecond>&&) = default;

		template<typename... Ts>
		inline cvpair(Ts&&... vs)
			: second(std::forward<Ts>(vs)...)
		{}
	};

	/*! Ordered pair composed of two runtime values */
	template<typename Tfirst, typename Tsecond>
	struct vvpair
	{
		typedef Tfirst first_type;
		typedef Tsecond second_type;

		Tfirst first;	
		Tsecond second;

		vvpair() = default;
		vvpair(const vvpair<Tfirst,Tsecond>&) = default;
		vvpair(vvpair<Tfirst,Tsecond>&&) = default;

		vvpair(const std::pair<Tfirst,Tsecond> &p)
			: first(p.first)
			, second(p.second)
		{}

		vvpair(std::pair<Tfirst,Tsecond> &&p)
			: first(std::move(p.first))
			, second(std::move(p.second))
		{}

		vvpair& operator=(const vvpair<Tfirst,Tsecond>&) = default;
		vvpair& operator=(vvpair<Tfirst,Tsecond>&&) = default;

		vvpair& operator=(const std::pair<Tfirst,Tsecond> &p)
		{
			first = p.first;
			second = p.second;
		}

		vvpair& operator=(std::pair<Tfirst,Tsecond> &&p)
		{
			first = std::move(p.first);
			second = std::move(p.second);
		}

		vvpair(const Tfirst& f, const Tsecond& s) 
			: first(f)
			, second(s)
		{}

		vvpair(const Tfirst& f, Tsecond&& s) 
			: first(f)
			, second(std::move(s))
		{}

		vvpair(Tfirst&& f, const Tsecond& s) 
			: first(std::move(f))
			, second(s)
		{}

		vvpair(Tfirst&& f, Tsecond&& s) 
			: first(std::move(f))
			, second(std::move(s))
		{}
	};
}
#endif
