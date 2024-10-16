#ifndef DEV_BDC_HH
# define DEV_BDC_HH
# include <drivers/TLE9201.h>
namespace dev
{
    class bdc
        : private TLE9201
    {
        
    public:
        
        bdc(SPI &s, GPIO_PIN c)
            : TLE9201(s,c)
        {}
                
        void operator()(float duty)
        {
            if(abs(duty) >= .5f)
                start(duty > 0);
            else
                stop();
        }
    };
}
#endif