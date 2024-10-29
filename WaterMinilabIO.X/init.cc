#include <array>
#include <thread>
#include <mutex>
#include <limits>
#include <condition_variable>
#include "definitions.h"
#include <io/host/service.hh>
#include <WML.hh>
#include <argtypes.hh>
#include "px_int.h"

extern "C" volatile uint32_t idle_count;


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
       
 
    io::host::log("Creating WML instance");
    auto &wml = *new WML();
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
    
    // get time
    io::host::default_earpc::set_command<int64_t,bool>(11,[](auto r) {
        r.respond(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count());
    });
    
    // get free memory bytes
    io::host::default_earpc::set_command<uint32_t,bool>(12,[](auto r) {
        ULONG avail = 0;
	    tx_byte_pool_info_get(&posix_heap_byte_pool, 0, &avail, 0, 0, 0, 0);
        r.respond(avail);
    });
    
    // get total memory bytes
    io::host::default_earpc::set_command<uint32_t,bool>(13,[](auto r) {
        r.respond(POSIX_HEAP_SIZE_IN_BYTES);
    });
    
    // get cpu usage
    std::mutex cpu_usage_lk;
    volatile uint32_t last_idle_count = idle_count;
    auto cpu_usage_ts = std::chrono::high_resolution_clock::now();
    io::host::default_earpc::set_command<float,bool>(14,[&](auto r) {
        std::unique_lock ul(cpu_usage_lk);
        
        auto t0 = std::chrono::high_resolution_clock::now();
        const auto idle = idle_count - last_idle_count;
        last_idle_count = idle_count;
        const auto dur = t0-cpu_usage_ts;
        cpu_usage_ts = t0;
        ul.unlock();
        const float cpu = std::clamp(1.f-float(idle)/std::chrono::duration_cast<std::chrono::microseconds>(dur).count()/6, 0.f, 1.f);
        r.respond(cpu);
    });
    
    
    //Home stepper
    io::host::default_earpc::set_command<bool,uint8_t>(99,[&wml](auto r) {
        auto item = r.value();
        if(item >= wml.stepper.size())
            r.respond(false);
        else
        {
            wml.stepper[item].home();
            r.respond(true);
        }
    });
    
    //Send stepper to target index
    io::host::default_earpc::set_command<bool,value_set_t<int32_t> >(100,[&wml](auto r) {
        auto x = r.value();
        if(x.item >= wml.stepper.size())
            r.respond(false);
        else
        {
            wml.stepper[x.item].setTargetIndex(x.value);
            r.respond(true);
        }
    });
    
    //Set stepper freewheel state
    io::host::default_earpc::set_command<bool,value_set_t<bool> >(101,[&wml](auto r) {
        auto x = r.value();
        if(x.item >= wml.stepper.size())
            r.respond(false);
        else
        {
            wml.stepper[x.item].setFreewheel(x.value);
            r.respond(true);
        }
    });
    
    // Reset stepper position
    io::host::default_earpc::set_command<bool,uint8_t>(102,[&wml](auto r) {
        auto item = r.value();
        if(item >= wml.stepper.size())
            r.respond(false);
        else
        {
            wml.stepper[item].resetPosition();
            r.respond(true);
        }
    });
    
    // Set max accel
    io::host::default_earpc::set_command<bool,value_set_t<uint32_t> >(103,[&wml](auto r) {
        auto x = r.value();
        if(x.item >= wml.stepper.size())
            r.respond(false);
        else
        {
            wml.stepper[x.item].setMaxAccel(x.value);
            r.respond(true);
        }
    });
    
    // Set max rpm
    io::host::default_earpc::set_command<bool,value_set_t<uint32_t> >(104,[&wml](auto r) {
        auto x = r.value();
        if(x.item >= wml.stepper.size())
            r.respond(false);
        else
        {
            wml.stepper[x.item].setMaxRpm(x.value);
            r.respond(true);
        }
    });
    
    // Get stepper position
    io::host::default_earpc::set_command<int32_t,uint8_t>(105,[&wml](auto r) {
        auto item = r.value();
        if(item >= wml.stepper.size())
            r.respond(std::numeric_limits<int32_t>::min());
        else
        { r.respond(wml.stepper[item].getPosition()); }
    });
    
    //set servo to angle
    io::host::default_earpc::set_command<bool,value_set_t<float> >(110,[&wml](auto r) {
        auto x = r.value();
        if(x.item >= wml.servo.size())
            r.respond(false);
        else
        {
            wml.servo[x.item].setAngle(x.value);
            r.respond(true);
        }
    });
    
    //set valve state
    io::host::default_earpc::set_command<bool,value_set_t<bool> >(120,[&wml](auto r) {
        auto x = r.value();
        if(x.item >= wml.valve.size())
            r.respond(false);
        else
        {
            wml.valve[x.item](x.value);
            r.respond(true);
        }
    });
    
    //set pwm duty
    io::host::default_earpc::set_command<bool,value_set_t<float> >(130,[&wml](auto r) {
        auto x = r.value();
        if(x.item >= wml.pwm.size())
            r.respond(false);
        else
        {
            wml.pwm[x.item](x.value);
            r.respond(true);

        }
    });
    
    //set bdc duty
    io::host::default_earpc::set_command<bool,value_set_t<float> >(140,[&wml](auto r) {
        auto x = r.value();
        if(x.item >= wml.bdc.size())
            r.respond(false);
        else
            r.respond(wml.bdc[x.item](x.value));
    });
        
    //get proximity
    io::host::default_earpc::set_command<int16_t,bool>(200,[&wml](auto r) {
        int16_t rv = -1;
        if(wml.proximity_sensor.read((uint16_t&)rv))
        {
            if(rv < 0)
                rv = std::numeric_limits<int16_t>::max();
            r.respond(rv);
        }
        else
            r.respond(-1);
    });
    
    //get limits
    io::host::default_earpc::set_command<uint8_t,bool>(201,[&wml](auto r) {
        uint8_t rv = 0;
        for(int i=0; i < wml.limit_sensor.size(); ++i)
            rv += (wml.limit_sensor[i]()==0)?(1<<i):0;
        
        r.respond(rv);
    });
    
    //get temp sensor
    io::host::default_earpc::set_command<uint16_t,bool>(202,[&wml](auto r) {
        uint16_t rv = 0;
        
        if(wml.temp_sensor.read(rv))
            r.respond(rv);
        else
            r.respond(0);
    });
    
    
    //get ADC channel
    io::host::default_earpc::set_command<float,uint8_t>(203,[&wml](auto r) {
        const auto item = r.value();
        if(item > 7)
        {
            r.respond(std::numeric_limits<float>::quiet_NaN());
            return;
        }
        
        if(!wml.adc.channel_select(item))
        {
            r.respond(std::numeric_limits<float>::quiet_NaN());
            return;
        }
        
        /*if(!wml.adc.sequence_config(0b00010000))
        {
            r.respond(std::numeric_limits<float>::quiet_NaN());
            return;
        }*/
        
        std::this_thread::sleep_for(std::chrono::milliseconds(3));
        
        uint16_t rv = 0;
        
        if(wml.adc.read(rv))
            r.respond(float(rv)/65535);
        else
            r.respond(std::numeric_limits<float>::quiet_NaN());
    });
    
    
    wml.adc.general_config(0b00000100);
    wml.adc.data_config(0b0001000);
    wml.adc.opmode_config(0b00001000);
    wml.adc.osr_config(0b00000111);
    
    io::host::log("Starting loop");
    while(true)
    {
        uint16_t rd;
        /*if(proximity_sensor.read(rd))
            io::host::log("Proximity: ",rd);
        else
            io::host::log("Proximity read failed");
        */
        // for(int i = 0; i < 3; ++i)
        //    io::host::log("Limit ",i,": ",wml.limit_sensor[i]());
      
        //tx_semaphore_get(&sem_index,TX_WAIT_FOREVER);
        //auto sgr = stepper[0].getStallGuardResult();
        
            //io::host::log("P: ",encoders[0].position,", ",encoders[3].position,", ",encoders[2].position);
            //io::host::log("Limit: ", limit_sensor[0](),", ",limit_sensor[1](),", ",limit_sensor[2]());
            
            //std::this_thread::sleep_for(std::chrono::milliseconds(20));
        //}
        
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        //io::host::log("ADC system status: ",std::hex,(int)wml.adc.system_status());
        
        /*float temp = 0.f;
        if(wml.temp_sensor.read(temp))
            io::host::log("Temp: ",temp);
        else
            io::host::log("Temperature read failed ");
        */
        //wml.adc.channel_select(2);
        /*wml.adc.data_config(128);
        for(int i = 3; i < 8; ++i)
        {
            wml.adc.channel_select(i);

            for(int j = 0; j < 3; ++j)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));

                uint16_t r = 0;
                if(wml.adc.read(r))
                    io::host::log("ADC ",i," read: ",std::hex,r);
                else
                    io::host::log("ADC ",i," read failed");
            }
        }*/
    }
}
