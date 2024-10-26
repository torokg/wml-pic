#ifndef WML_HH
# define WML_HH
# include <array>
# include <drivers/i2c.hh>
# include <drivers/uart.hh>
# include <drivers/MCP23S17.h>
# include <drivers/PCA9536.h>
# include <drivers/PCA9546.h>
# include <drivers/VNCL4040.h>
# include <drivers/TLA2518.h>
# include <drivers/MAX31865.h>
# include <drivers/LimitSensor.hh>
# include <drivers/encoder.h>
# include <dev/stepper.hh>
# include <dev/servo.hh>
# include <dev/valve.hh>
# include <dev/pwm.hh>
# include <dev/bdc.hh>
struct WML
{
    std::array<HardwareSerial,6> uart;
    std::array<oc_type,5>        oc;
    std::array<SPI,3>            spi;
    std::array<NativeI2C,2>      i2c;
    
    MCP23S17                     portexp;
    std::array<PCA9536,2>        i2c_portexp;
    std::array<PCA9546,2>        i2c_mux;
    std::array<LimitSensor, 3>   limit_sensor;
    VNCL4040                     proximity_sensor;
    TLA2518                      adc;
    MAX31865                     temp_sensor;
    std::array<dev::servo, 3>    servo;
    std::array<dev::pwm,2>       pwm;
    std::array<dev::valve, 12>   valve;
    std::array<dev::bdc, 4>      bdc;
    std::array<dev::stepper,10>  stepper;
    

    WML();
};
#endif