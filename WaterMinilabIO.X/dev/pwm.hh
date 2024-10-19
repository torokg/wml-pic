#ifndef DEV_PWM_HH
# define DEV_PWM_HH
# include <drivers/oc.hh>
namespace dev
{
    
class pwm
{
    oc_type &_;
    
public:
    inline pwm(oc_type &oc)
        : _(oc)
    {}
    
    inline void operator()(float duty)
    {
        const int x = (int)_.period/1000;
        _.set((uint16_t)(std::clamp(1.f-duty,0.f,1.f)*_.period));
    }

};

}
#endif