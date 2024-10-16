#include <array>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "definitions.h"
#include <io/host/service.hh>
#include <drivers/MCP23S17.h>
#include <dev/stepper.hh>
#include <dev/servo.hh>
#include <dev/valve.hh>
#include <dev/pwm.hh>

template<typename T>
struct value_set_t
{
    T value;
    uint8_t item;
};
    

 
static HardwareSerial uart[6] = {
        {UART1_SerialSetup, UART1_ReadCountGet, UART1_Read, UART1_Write, UART1_Flush},
        {UART2_SerialSetup, UART2_ReadCountGet, UART2_Read, UART2_Write, UART2_Flush},
        {UART3_SerialSetup, UART3_ReadCountGet, UART3_Read, UART3_Write, UART3_Flush},
        {UART4_SerialSetup, UART4_ReadCountGet, UART4_Read, UART4_Write, UART4_Flush},
        {UART5_SerialSetup, UART5_ReadCountGet, UART5_Read, UART5_Write, UART5_Flush},
        {UART6_SerialSetup, UART6_ReadCountGet, UART6_Read, UART6_Write, UART6_Flush},
        
    };

 
static oc_type oc[5] = {
    {OCMP1_CompareValueSet, 37499},
    {OCMP9_CompareValueSet, 37499},
    {OCMP5_CompareValueSet, 37499},
    {OCMP3_CompareValueSet, 49998},
    {OCMP4_CompareValueSet, 49998},
};
    
static SPI spi[3] = {
    {SPI2_Read, SPI2_Write, SPI2_WriteRead},
    {SPI5_Read, SPI5_Write, SPI5_WriteRead},
    {SPI6_Read, SPI6_Write, SPI6_WriteRead}
};
    

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
    
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
   
    TMR2_Start();
    TMR2_InterruptEnable();
    
    TMR3_Start();
    TMR3_InterruptEnable();
    
    auto &portexp = *new MCP23S17(spi[1], SPI5_CS_PIN, SPI5_RESET_PIN);
            
    auto &stepper = *new std::array<dev::stepper,6> {
        dev::stepper{uart[2], TMC2209::SERIAL_ADDRESS_0, STX_ENABLE_PIN, STX_INDEX_PIN, STX_DIAG_PIN, 100UL, 10UL, 200UL, 1000, 300}, 
        dev::stepper{uart[0], TMC2209::SERIAL_ADDRESS_0, STY_ENABLE_PIN, STY_INDEX_PIN, STY_DIAG_PIN,  90UL,  9UL, 200UL, 1000, 300},
        dev::stepper{uart[3], TMC2209::SERIAL_ADDRESS_0, STZ_ENABLE_PIN, STZ_INDEX_PIN, STZ_DIAG_PIN, 100UL, 10UL, 200UL,  500, 300},
        dev::stepper{uart[1], TMC2209::SERIAL_ADDRESS_0, STC_ENABLE_PIN, STC_INDEX_PIN, STC_DIAG_PIN,  60UL,  10UL, 200UL, 500, 300},
        //{uart[5], TMC2209::SERIAL_ADDRESS_0, ST4_ENABLE_PIN, ST4_INDEX_PIN, ST4_DIAG_PIN,  40UL,  40UL, 200UL, 2000, 10000},
        //{uart[5], TMC2209::SERIAL_ADDRESS_1, ST5_ENABLE_PIN, ST5_INDEX_PIN, ST5_DIAG_PIN,  40UL,  40UL, 200UL, 2000, 10000},
        //{uart[5], TMC2209::SERIAL_ADDRESS_2, ST6_ENABLE_PIN, ST6_INDEX_PIN, ST6_DIAG_PIN,  40UL,  40UL, 200UL, 2000, 10000},
        //{uart[5], TMC2209::SERIAL_ADDRESS_3, ST7_ENABLE_PIN, ST7_INDEX_PIN, ST7_DIAG_PIN,  40UL,  40UL, 200UL, 2000, 10000},
        dev::stepper{uart[4], TMC2209::SERIAL_ADDRESS_0, ST8_ENABLE_PIN, ST8_INDEX_PIN, ST8_DIAG_PIN,  40UL,  40UL, 200UL, 2000, 200},
        dev::stepper{uart[4], TMC2209::SERIAL_ADDRESS_2, ST9_ENABLE_PIN, ST9_INDEX_PIN, ST9_DIAG_PIN,  40UL,  40UL, 200UL, 2000, 200}
    };
    
    auto &servo = *new std::array<dev::servo, 3> {
        dev::servo{oc[0], 1.0f/6, 0.4},
        dev::servo{oc[1], 1.0f/6, 5.0f/6},
        dev::servo{oc[2], 1.0f/6, 5.0f/6}
    };
    
    auto &pwm = *new std::array<dev::pwm,2>{
        dev::pwm{oc[3]},
        dev::pwm{oc[4]}
    };
    
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
    
    /*auto &bdc = *new std::array<dev::bdc, 4> {
        dev::bdc{spi[0],SPI2_CS0_PIN},
        dev::bdc{spi[0],SPI2_CS0_PIN},
        dev::bdc{spi[0],SPI2_CS0_PIN},
    
    };*/
    std::mutex lk;
    std::condition_variable cv;
    io::host::log("Setting earpc commands");

    
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
            r.respond(true);
            pwm[x.item](x.value);
        }
    });
    
    
    while(true)
    {
        //tx_semaphore_get(&sem_index,TX_WAIT_FOREVER);
        //auto sgr = stepper[0].getStallGuardResult();
        //io::host::log("STY index count: ",sty_index_count,"; stall guard result: ",sgr);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}
