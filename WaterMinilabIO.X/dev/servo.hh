#ifndef DEV_SERVO_HH
# define DEV_SERVO_HH
# include <algorithm>
# include <cstdint>
# include <cstddef>
# include <drivers/oc.hh>
namespace dev
{

    
class servo
{
    oc_type _;
    const float lob;
    const float hib;
    
public:

    inline void setAngle(float angle)
    {
        angle = std::clamp(angle,-1.f,1.f)+1.f;
        angle /= 2;
        angle *= hib-lob;
        angle += lob;
        _.set((uint16_t)(angle*_.period));
    }
    
    inline servo(oc_type o, float l, float h)
       : _(o)
       , lob(l)
       , hib(h)
    { setAngle(0); }
       
};

}
#endif
