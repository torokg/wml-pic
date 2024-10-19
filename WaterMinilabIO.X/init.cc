#include <array>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "definitions.h"
#include <io/host/service.hh>
#include <drivers/i2c.hh>
#include <drivers/uart.hh>
#include <drivers/MCP23S17.h>
#include <drivers/PCA9536.h>
#include <drivers/PCA9546.h>
#include <drivers/A31301.h>
#include <drivers/VNCL4040.h>
#include <drivers/encoder.h>
#include <dev/stepper.hh>
#include <dev/servo.hh>
#include <dev/valve.hh>
#include <dev/pwm.hh>
#include <dev/bdc.hh>


template<typename T>
struct value_set_t
{
    T value;
    uint8_t item;
};


template<typename... Ts>
float distance(Ts&&... vs)
{ return powf(((float(vs)*float(vs)) + ...), 1.f/sizeof...(Ts)); }

void init()
{
    if(!io::host::service::init())
    {
        __builtin_software_breakpoint();
    }
    
	UART1_Initialize();

	UART2_Initialize();
    
    UART3_Initialize();

	UART4_Initialize();

	UART5_Initialize();

	UART6_Initialize();
    
    
    SPI2_Initialize();
    
	SPI5_Initialize();
    
    SPI6_Initialize();
    
    
    I2C1_Initialize();
    
    I2C2_Initialize();
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
       
 
    auto &uart = *new std::array<HardwareSerial,6>{
        HardwareSerial{UART1_SerialSetup, UART1_ReadCountGet, UART1_Read, UART1_Write, UART1_Flush},
        HardwareSerial{UART2_SerialSetup, UART2_ReadCountGet, UART2_Read, UART2_Write, UART2_Flush},
        HardwareSerial{UART3_SerialSetup, UART3_ReadCountGet, UART3_Read, UART3_Write, UART3_Flush},
        HardwareSerial{UART4_SerialSetup, UART4_ReadCountGet, UART4_Read, UART4_Write, UART4_Flush},
        HardwareSerial{UART5_SerialSetup, UART5_ReadCountGet, UART5_Read, UART5_Write, UART5_Flush},
        HardwareSerial{UART6_SerialSetup, UART6_ReadCountGet, UART6_Read, UART6_Write, UART6_Flush},
    };

    auto &oc = *new std::array<oc_type,5>{
        oc_type{OCMP1_CompareValueSet, 37499},
        oc_type{OCMP9_CompareValueSet, 37499},
        oc_type{OCMP5_CompareValueSet, 37499},
        oc_type{OCMP3_CompareValueSet, 50000},
        oc_type{OCMP4_CompareValueSet, 50000},
    };
    
    auto &spi = *new std::array<SPI,3>{
        SPI{SPI2_Read, SPI2_Write, SPI2_WriteRead,{}},
        SPI{SPI5_Read, SPI5_Write, SPI5_WriteRead,{}},
        SPI{SPI6_Read, SPI6_Write, SPI6_WriteRead,{}}
    };
    
    auto &i2c = *new std::array<NativeI2C,2>{
        NativeI2C{I2C1_Read,I2C1_Write,I2C1_WriteRead},
        NativeI2C{I2C2_Read,I2C2_Write,I2C2_WriteRead}
    };
    
   
    TMR2_Start();
    TMR2_InterruptEnable();
    
    TMR3_Start();
    TMR3_InterruptEnable();
    
    io::host::log("Starting MCP23S17");
    auto &portexp = *new MCP23S17(spi[1], SPI5_CS_PIN, SPI5_RESET_PIN);
    
             
    io::host::log("Starting all TMC2209");
    auto &stepper = *new std::array<dev::stepper,6> {
        dev::stepper{uart[2], TMC2209::SERIAL_ADDRESS_0, stepper_index[0], &encoders[0], STX_ENABLE_PIN, STX_DIAG_PIN, 100UL, 10UL, 200UL, 1000, 300}, 
        dev::stepper{uart[0], TMC2209::SERIAL_ADDRESS_0, stepper_index[1], &encoders[3], STY_ENABLE_PIN, STY_DIAG_PIN,  90UL,  9UL, 200UL, 1000, 300},   // limit sensor: ; home: 
        dev::stepper{uart[3], TMC2209::SERIAL_ADDRESS_0, stepper_index[2], &encoders[2], STZ_ENABLE_PIN, STZ_DIAG_PIN, 100UL, 10UL, 200UL,  500, 300},   // limit sensor: 2; home: z < -620
        dev::stepper{uart[1], TMC2209::SERIAL_ADDRESS_0, stepper_index[3], 0, STC_ENABLE_PIN, STC_DIAG_PIN,  60UL,  10UL, 200UL, 500, 300},
        //{uart[5], TMC2209::SERIAL_ADDRESS_0, stepper_index_handler[4], 0, ST4_ENABLE_PIN,  ST4_DIAG_PIN,  40UL,  40UL, 200UL, 2000, 10000},
        //{uart[5], TMC2209::SERIAL_ADDRESS_1, stepper_index_handler[5], 0, ST5_ENABLE_PIN, ST5_DIAG_PIN,  40UL,  40UL, 200UL, 2000, 10000},
        //{uart[5], TMC2209::SERIAL_ADDRESS_2, stepper_index_handler[6], 0, ST6_ENABLE_PIN, ST6_DIAG_PIN,  40UL,  40UL, 200UL, 2000, 10000},
        //{uart[5], TMC2209::SERIAL_ADDRESS_3, stepper_index_handler[7], 0, ST7_ENABLE_PIN, ST7_DIAG_PIN,  40UL,  40UL, 200UL, 2000, 10000},
        dev::stepper{uart[4], TMC2209::SERIAL_ADDRESS_0, stepper_index[8], 0, ST8_ENABLE_PIN, ST8_DIAG_PIN,  40UL,  40UL, 200UL, 2000, 200},
        dev::stepper{uart[4], TMC2209::SERIAL_ADDRESS_2, stepper_index[9], 0, ST9_ENABLE_PIN, ST9_DIAG_PIN,  40UL,  40UL, 200UL, 2000, 200}
    };
    
    io::host::log("Starting all servos");
    auto &servo = *new std::array<dev::servo, 3> {
        dev::servo{oc[0], 1.0f/6, 0.4},
        dev::servo{oc[1], 1.0f/6, 5.0f/6},
        dev::servo{oc[2], 1.0f/6, 5.0f/6}
    };
    
    io::host::log("Starting all pwm outputs");
    auto &pwm = *new std::array<dev::pwm,2>{
        dev::pwm{oc[3]},
        dev::pwm{oc[4]}
    };
    
    io::host::log("Starting all valve controls");
    auto &valve = *new std::array<dev::valve, 12>{
        dev::valve{portexp,0},
        dev::valve{portexp,1},
        dev::valve{portexp,2},
        dev::valve{portexp,3},
        dev::valve{portexp,4},
        dev::valve{portexp,5},
        dev::valve{portexp,6},
        dev::valve{portexp,7},
        dev::valve{portexp,8},
        dev::valve{portexp,9},
        dev::valve{portexp,10},
        dev::valve{portexp,11},
    };
    
    io::host::log("Starting all bdc controllers");
    auto &bdc = *new std::array<dev::bdc, 4> {
        dev::bdc{spi[0],SPI2_CS0_PIN},
        dev::bdc{spi[0],SPI2_CS1_PIN},
        dev::bdc{spi[0],SPI2_CS2_PIN},
        dev::bdc{spi[0],SPI2_CS3_PIN},
    };
    
    
    io::host::log("Starting all PCA9536");
    auto &i2c_portexp = *new std::array<PCA9536,2>{
        PCA9536{i2c[0],0b1000001},
        PCA9536{i2c[1],0b1000001}
    };
    
    io::host::log("Starting all PCA9546");
    auto &i2c_mux = *new std::array<PCA9546,2>{
        PCA9546{i2c[0],0b1110000},
        PCA9546{i2c[1],0b1110000}
    };
    
    io::host::log("Turning on i2c hub 0");
    if(!i2c_portexp[0].value(0))
        io::host::log("Failed to turn on i2c hub 0");
    
    io::host::log("Turning on i2c hub 1");
    if(!i2c_portexp[1].value(0))
        io::host::log("Failed to turn on i2c hub 1");
    
    io::host::log("Starting all A31301");
    auto &limit_sensor = *new std::array<A31301, 3>{
        A31301{i2c_mux[0][0],0x6c},
        A31301{i2c_mux[0][1],0x6c},
        A31301{i2c_mux[0][2],0x6c},
    };
    
    io::host::log("Starting VNCL4040");
    auto &proximity_sensor = *new VNCL4040(i2c_mux[0][3],0x60);
    
    
    io::host::log("Setting earpc commands");
    
    
    //reboot
    io::host::default_earpc::set_command<bool,bool>(10,[](auto r) {
        r.respond(true);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        // starting critical sequence
        SYSKEY = 0x00000000; //write invalid key to force lock
        SYSKEY = 0xAA996655; //write key1 to SYSKEY
        SYSKEY = 0x556699AA; //write key2 to SYSKEY
        // OSCCON is now unlocked

        /* set SWRST bit to arm reset */
        RSWRSTSET = 1;
        volatile uint32_t y = RSWRST;
    });
    
    //Send stepper to target index
    io::host::default_earpc::set_command<bool,value_set_t<int32_t> >(100,[&stepper](auto r) {
        auto x = r.value();
        if(x.item >= stepper.size())
            r.respond(false);
        else
        {
            stepper[x.item].setTargetIndex(x.value);
            r.respond(true);
        }
    });
        
    //set servo to angle
    io::host::default_earpc::set_command<bool,value_set_t<float> >(110,[&servo](auto r) {
        auto x = r.value();
        if(x.item >= servo.size())
            r.respond(false);
        else
        {
            servo[x.item].setAngle(x.value);
            r.respond(true);
        }
    });
    
    //set valve state
    io::host::default_earpc::set_command<bool,value_set_t<bool> >(120,[&valve](auto r) {
        auto x = r.value();
        if(x.item >= valve.size())
            r.respond(false);
        else
        {
            valve[x.item](x.value);
            r.respond(true);
        }
    });
    
    //set pwm duty
    io::host::default_earpc::set_command<bool,value_set_t<float> >(130,[&pwm](auto r) {
        auto x = r.value();
        if(x.item >= pwm.size())
            r.respond(false);
        else
        {
            pwm[x.item](x.value);
            r.respond(true);

        }
    });
    
    //set bdc duty
    io::host::default_earpc::set_command<bool,value_set_t<float> >(140,[&bdc](auto r) {
        auto x = r.value();
        if(x.item >= bdc.size())
            r.respond(false);
        else
            r.respond(bdc[x.item](x.value));
    });
    
    io::host::log("Starting loop");
    while(true)
    {
/*        uint16_t rd;
        if(proximity_sensor.read(rd))
            io::host::log("Proximity: ",rd);
        else
            io::host::log("Proximity read failed");
  */      
        /*for(int i = 0; i < 3; ++i)
        {
            A31301::result lr;
            memset(&lr,0,sizeof(A31301::result));
            if(limit_sensor[i].read(lr))
                io::host::log("Limit ",i,": ", lr.x ,'\t', lr.y, '\t',lr.z
                        //distance(limit_result.x,limit_result.y,limit_result.z)
                );
            //else
            //    io::host::log("Limit ",i," read failed");
        }*/
        
        //tx_semaphore_get(&sem_index,TX_WAIT_FOREVER);
        //auto sgr = stepper[0].getStallGuardResult();
        
        //for(int i = 0; i < 4; ++i)
        //{
        //int i=2;
            //io::host::log("Encoder",i,": ",encoders[i].position,"; ",(int)encoders[i].state[0],' ',(int)encoders[i].state[1],' ',(int)encoders[i].state[2],' ',(int)encoders[i].state[3],' ');
        
            io::host::log("Position: ",encoders[0].position,", ",encoders[3].position,", ",encoders[2].position);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        //}
        //std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}
