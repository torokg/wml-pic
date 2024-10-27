#include "WML.hh"
#include <io/host/service.hh>
#include <argtypes.hh>

void
WML::stepper_finish_callback(uint8_t i)
{
    std::mutex lk;
    std::condition_variable cv;
    
    std::unique_lock ul(stepper_finish_lk[i]);
    const auto cnt = stepper_finish_counter[i];
    io::host::default_earpc::call<bool,value_set_t<uint32_t>>(0,100,{cnt,i},[&](auto r)
    {
        stepper_finish_counter[i] = cnt + 1;
        cv.notify_all();
    });
    
    std::unique_lock ul2(lk);
    cv.wait(ul2);
}
    
WML::WML()
    : stepper_finish_counter{0U,0U,0U,0U,0U,0U,0U,0U,0U,0U}
    , uart
       { HardwareSerial{UART1_SerialSetup, UART1_ReadCountGet, UART1_Read, UART1_Write, UART1_Flush}
       , HardwareSerial{UART2_SerialSetup, UART2_ReadCountGet, UART2_Read, UART2_Write, UART2_Flush}
       , HardwareSerial{UART3_SerialSetup, UART3_ReadCountGet, UART3_Read, UART3_Write, UART3_Flush}
       , HardwareSerial{UART4_SerialSetup, UART4_ReadCountGet, UART4_Read, UART4_Write, UART4_Flush}
       , HardwareSerial{UART5_SerialSetup, UART5_ReadCountGet, UART5_Read, UART5_Write, UART5_Flush}
       , HardwareSerial{UART6_SerialSetup, UART6_ReadCountGet, UART6_Read, UART6_Write, UART6_Flush}
       }
    , oc
       { oc_type{OCMP1_CompareValueSet, 37499}
       , oc_type{OCMP9_CompareValueSet, 37499}
       , oc_type{OCMP5_CompareValueSet, 37499}
       , oc_type{OCMP3_CompareValueSet, 50000}
       , oc_type{OCMP4_CompareValueSet, 50000}
       }
    , spi
       { SPI{SPI2_Read, SPI2_Write, SPI2_WriteRead,{}},
         SPI{SPI5_Read, SPI5_Write, SPI5_WriteRead,{}},
         SPI{SPI6_Read, SPI6_Write, SPI6_WriteRead,{}}
       }
    , i2c
       { NativeI2C{I2C1_Read,I2C1_Write,I2C1_WriteRead}
       , NativeI2C{I2C2_Read,I2C2_Write,I2C2_WriteRead}
       }
    , portexp
       { spi[1], SPI5_CS_PIN, SPI5_RESET_PIN }
    , i2c_portexp
       { PCA9536{i2c[0],0b1000001}
       , PCA9536{i2c[1],0b1000001}
       }
    , i2c_mux
       {
        PCA9546{i2c[0],0b1110000},
        PCA9546{i2c[1],0b1110000}
       }
    
    , limit_sensor
       { LimitSensor{i2c_mux[0][0],0x6c, 2, -1131, -1117, false}
       , LimitSensor{i2c_mux[0][1],0x6c, 2,  191,  205, true}
       , LimitSensor{i2c_mux[0][2],0x6c, 2, -481, -467, false}
       }
    , proximity_sensor
       ( i2c_mux[0][3], 0x60 )
    , adc
       ( spi[2], SPI6_CS1_PIN )
    , temp_sensor
       ( spi[2], SPI6_CS0_PIN)
    , servo
       { dev::servo{oc[0], 1.0f/6, 0.4}
       , dev::servo{oc[1], 1.0f/6, 5.0f/6}
       , dev::servo{oc[2], 1.0f/6, 5.0f/6}
       }
    , pwm
       { dev::pwm{oc[3]}
       , dev::pwm{oc[4]}
       }
    , valve
       { dev::valve{portexp,0}
       , dev::valve{portexp,1}
       , dev::valve{portexp,2}
       , dev::valve{portexp,3}
       , dev::valve{portexp,4}
       , dev::valve{portexp,5}
       , dev::valve{portexp,6}
       , dev::valve{portexp,7}
       , dev::valve{portexp,8}
       , dev::valve{portexp,9}
       , dev::valve{portexp,10}
       , dev::valve{portexp,11}
       }
    , bdc
       { dev::bdc{spi[0],SPI2_CS0_PIN}
       , dev::bdc{spi[0],SPI2_CS1_PIN}
       , dev::bdc{spi[0],SPI2_CS2_PIN}
       , dev::bdc{spi[0],SPI2_CS3_PIN}
       }
    , stepper                                                                                                                                                                // Irun Ihold  SpR,   RPM  Acc
       { dev::stepper{uart[2], TMC2209::SERIAL_ADDRESS_0, stepper_index[0], &encoders[0], &limit_sensor[0], std::bind(&WML::stepper_finish_callback,this,0), STX_ENABLE_PIN, STX_DIAG_PIN, 50UL,  5UL, 200UL, 500, 400}
       , dev::stepper{uart[0], TMC2209::SERIAL_ADDRESS_0, stepper_index[1], &encoders[3], &limit_sensor[1], std::bind(&WML::stepper_finish_callback,this,1), STY_ENABLE_PIN, STY_DIAG_PIN, 50UL,  5UL, 200UL, 500, 400}   // limit sensor: ; home: 
       , dev::stepper{uart[3], TMC2209::SERIAL_ADDRESS_0, stepper_index[2], &encoders[2], &limit_sensor[2], std::bind(&WML::stepper_finish_callback,this,2), STZ_ENABLE_PIN, STZ_DIAG_PIN, 50UL,  5UL, 200UL, 600, 500}   // limit sensor: 2; home: z < -620
       , dev::stepper{uart[1], TMC2209::SERIAL_ADDRESS_0, stepper_index[3],            0,                0, std::bind(&WML::stepper_finish_callback,this,3), STC_ENABLE_PIN, STC_DIAG_PIN, 60UL,  0UL, 200UL, 300, 50}
       , dev::stepper{uart[5], TMC2209::SERIAL_ADDRESS_0, stepper_index[4],            0,                0, std::bind(&WML::stepper_finish_callback,this,4), ST4_ENABLE_PIN, ST4_DIAG_PIN, 40UL, 40UL, 200UL, 300, 400}
       , dev::stepper{uart[5], TMC2209::SERIAL_ADDRESS_2, stepper_index[5],            0,                0, std::bind(&WML::stepper_finish_callback,this,5), ST5_ENABLE_PIN, ST5_DIAG_PIN, 40UL, 40UL, 200UL, 300, 400}
       , dev::stepper{uart[5], TMC2209::SERIAL_ADDRESS_1, stepper_index[6],            0,                0, std::bind(&WML::stepper_finish_callback,this,6), ST6_ENABLE_PIN, ST6_DIAG_PIN, 40UL, 40UL, 200UL, 300, 400}
       , dev::stepper{uart[5], TMC2209::SERIAL_ADDRESS_3, stepper_index[7],            0,                0, std::bind(&WML::stepper_finish_callback,this,7), ST7_ENABLE_PIN, ST7_DIAG_PIN, 40UL, 40UL, 200UL, 300, 400}
       , dev::stepper{uart[4], TMC2209::SERIAL_ADDRESS_0, stepper_index[8],            0,                0, std::bind(&WML::stepper_finish_callback,this,8), ST8_ENABLE_PIN, ST8_DIAG_PIN, 40UL, 40UL, 200UL, 300, 400}
       , dev::stepper{uart[4], TMC2209::SERIAL_ADDRESS_2, stepper_index[9],            0,                0, std::bind(&WML::stepper_finish_callback,this,9), ST9_ENABLE_PIN, ST9_DIAG_PIN, 40UL, 40UL, 200UL, 300, 400}
       }
{
    
    TMR2_Start();
    TMR2_InterruptEnable();
    
    TMR3_Start();
    TMR3_InterruptEnable();
    
    //io::host::log("Starting MCP23S17");
    
    //io::host::log("Starting all PCA9536");
    //io::host::log("Starting all PCA9546");
    
    //io::host::log("Turning on i2c hub 0");
    //if(!i2c_portexp[0].value(0))
    //    io::host::log("Failed to turn on i2c hub 0");
    
    //io::host::log("Turning on i2c hub 1");
    //if(!i2c_portexp[1].value(0))
    //    io::host::log("Failed to turn on i2c hub 1");
    
    //io::host::log("Starting all limit sensors");

    //io::host::log("Starting VNCL4040");
    
    //io::host::log("Starting TLA2518");
    
    //io::host::log("Starting all servos");
    
    //io::host::log("Starting all pwm outputs");
    
    //io::host::log("Starting all valve controls");
    
    //io::host::log("Starting all bdc controllers");
    
    //io::host::log("Starting all TMC2209");
}