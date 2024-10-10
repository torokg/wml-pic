#ifndef NET_IPV4_ADDRESS_H
# define NET_IPV4_ADDRESS_H
# include <cstdint>
# include <sstream>
namespace net 
{
#pragma pack(push,1)
	struct
	ipv4_address
	{
		uint8_t octet[4];

		ipv4_address() {}

		ipv4_address(uint32_t ip)
			: octet{
				static_cast<uint8_t>((ip>>24)&0xff),
				static_cast<uint8_t>((ip>>16)&0xff),
				static_cast<uint8_t>((ip>>8)&0xff),
				static_cast<uint8_t>(ip&0xff)
			}
		{}

		ipv4_address(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
		{ octet[0] = a; octet[1] = b; octet[2] = c; octet[3] = d; }

		operator uint32_t() const
		{
			uint32_t v;
			for(int i = 0; i < 4; ++i)
				reinterpret_cast<uint8_t*>(&v)[3-i] = octet[i];
			return v;
		}

		const ipv4_address &operator=(uint32_t v)
		{ 
			for(int i = 0; i < 4; ++i)
				octet[i] = reinterpret_cast<uint8_t*>(&v)[3-i];

			return *this;
		}

		bool operator==(uint32_t v) const
		{
			for(int i = 0; i < 4; ++i)
				if(octet[i] != reinterpret_cast<uint8_t*>(&v)[3-i])
					return false;
			return true;
		}

		bool operator!=(uint32_t v) const
		{
			for(int i = 0; i < 4; ++i)
				if(octet[i] != reinterpret_cast<uint8_t*>(&v)[3-i])
					return true;
			return false;
		}

		bool operator==(ipv4_address that) const
		{
			for(int i = 0; i < 4; ++i)
				if(octet[i] != that.octet[i])
					return false;
			return true;
		}

		bool operator!=(ipv4_address that) const
		{
			for(int i = 0; i < 4; ++i)
				if(octet[i] != that.octet[i])
					return true;
			return false;
		}
		
		explicit operator std::string() const
		{
			std::stringstream r;
			r << std::dec << (int)octet[0] << '.' <<
				(int)octet[1] << '.' <<
				(int)octet[2] << '.' <<
				(int)octet[3];
			return r.str();
		}
	};
#pragma pack(pop)
}

#endif

