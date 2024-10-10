#ifndef NET_MAC_ADDRESS_H
# define NET_MAC_ADDRESS_H
# include <cstdint>
namespace net
{
#pragma pack(push,1)
	struct
	mac_address
	{
		uint8_t octet[6];

		constexpr mac_address() : octet{0,0,0,0,0,0} {}

		mac_address(uint8_t o0,uint8_t o1,uint8_t o2,uint8_t o3,uint8_t o4,uint8_t o5)
		{
			octet[0] = o0;
			octet[1] = o1;
			octet[2] = o2;
			octet[3] = o3;
			octet[4] = o4;
			octet[5] = o5;
		}

		bool operator==(const mac_address &that)
		{
			for(int i = 0; i < 6; ++i)
				if(that.octet[i] != octet[i])
					return false;
			return true;
		}

		bool operator!=(const mac_address &that)
		{
			for(int i = 0; i < 6; ++i)
				if(that.octet[i] != octet[i])
					return true;
			return false;
		}
	};
#pragma pack(pop)
}
#endif
