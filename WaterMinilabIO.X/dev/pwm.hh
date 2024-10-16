#ifndef DEV_PWM_HH
# define DEV_PWM_HH
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
        _.set((uint16_t)((1.f-duty)*_.period));
    }

};

}
#endif