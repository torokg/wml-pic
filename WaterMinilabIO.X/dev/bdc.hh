#ifndef DEV_BDC_HH
# define DEV_BDC_HH
namespace dev
{
    class bdc
        : public TLE9201
    {
        
    public:
        
        bdc(SPI &s, GPIO_PIN c)
            : TLE9201(s,c)
        {}
                
        spin(bool direction, float duty);
        
        stop();
    }
}
#endif