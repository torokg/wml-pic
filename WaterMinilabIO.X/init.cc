#include <thread>
#include <mutex>
#include <condition_variable>
#include <io/host/service.hh>
#include <dev/stepper.hh>

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
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    
    HardwareSerial uart[6] = {
        {UART1_SerialSetup, UART1_ReadCountGet, UART1_Read, UART1_Write, UART1_Flush},
        {UART2_SerialSetup, UART2_ReadCountGet, UART2_Read, UART2_Write, UART2_Flush},
        {UART3_SerialSetup, UART3_ReadCountGet, UART3_Read, UART3_Write, UART3_Flush},
        {UART4_SerialSetup, UART4_ReadCountGet, UART4_Read, UART4_Write, UART4_Flush},
        {UART5_SerialSetup, UART5_ReadCountGet, UART5_Read, UART5_Write, UART5_Flush},
        {UART6_SerialSetup, UART6_ReadCountGet, UART6_Read, UART6_Write, UART6_Flush},
        
    };
    
    dev::stepper stepper[6] = {
        {uart[2], TMC2209::SERIAL_ADDRESS_0, STX_ENABLE_PIN, STX_INDEX_PIN, STX_DIAG_PIN, 100UL, 10UL, 200UL, 1500, 10000}, 
        {uart[0], TMC2209::SERIAL_ADDRESS_0, STY_ENABLE_PIN, STY_INDEX_PIN, STY_DIAG_PIN,  90UL,  9UL, 200UL, 2000, 10000},
        {uart[3], TMC2209::SERIAL_ADDRESS_0, STZ_ENABLE_PIN, STZ_INDEX_PIN, STZ_DIAG_PIN, 100UL, 10UL, 200UL, 1500, 10000},
        {uart[1], TMC2209::SERIAL_ADDRESS_0, STC_ENABLE_PIN, STC_INDEX_PIN, STC_DIAG_PIN,  50UL,  5UL, 200UL, 2000, 10000},
        //{uart[5], TMC2209::SERIAL_ADDRESS_0, ST4_ENABLE_PIN, ST4_INDEX_PIN, ST4_DIAG_PIN,  40UL,  40UL, 200UL, 2000, 10000},
        //{uart[5], TMC2209::SERIAL_ADDRESS_1, ST5_ENABLE_PIN, ST5_INDEX_PIN, ST5_DIAG_PIN,  40UL,  40UL, 200UL, 2000, 10000},
        //{uart[5], TMC2209::SERIAL_ADDRESS_2, ST6_ENABLE_PIN, ST6_INDEX_PIN, ST6_DIAG_PIN,  40UL,  40UL, 200UL, 2000, 10000},
        //{uart[5], TMC2209::SERIAL_ADDRESS_3, ST7_ENABLE_PIN, ST7_INDEX_PIN, ST7_DIAG_PIN,  40UL,  40UL, 200UL, 2000, 10000},
        {uart[4], TMC2209::SERIAL_ADDRESS_0, ST8_ENABLE_PIN, ST8_INDEX_PIN, ST8_DIAG_PIN,  40UL,  40UL, 200UL, 2000, 10000},
        {uart[4], TMC2209::SERIAL_ADDRESS_2, ST9_ENABLE_PIN, ST9_INDEX_PIN, ST9_DIAG_PIN,  40UL,  40UL, 200UL, 2000, 10000}
    };
    
    io::host::log("Setting speed");
    for(int i = 0; i < 6; ++i)
        stepper[i].moveAtVelocity(10000);
    
    std::mutex lk;
    std::condition_variable cv;
    io::host::log("Setting earpc commands");
    
    struct velocity_set_t
    {
        int32_t velocity;
        uint8_t stepper;
    };
    //Set velocity
    io::host::default_earpc::set_command<bool,velocity_set_t>(100,[&stepper](auto r) {
        auto x = r.value();
        if(x.stepper >= 6)
            r.respond(false);
        else
        {
            if(x.velocity)
                stepper[x.stepper].enable();
            else
                stepper[x.stepper].disable();
            stepper[x.stepper].moveAtVelocity(x.velocity);
            r.respond(true);
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
