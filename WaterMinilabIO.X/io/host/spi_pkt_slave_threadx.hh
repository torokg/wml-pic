#ifndef IO_HOST_SPI_PKT_SLAVE_HH    /* Guard against multiple inclusion */
#define IO_HOST_SPI_PKT_SLAVE_HH

#include <cassert>
#include <cstring>
#include <limits>
#include <deque>
#include <vector>

#include "definitions.h"
#include <peripheral/spi/spi_slave/plib_spi_slave_common.h>
extern "C" long long debug_long0;

namespace io::host
{
    
typedef size_t(*spi_op_t)(void*,size_t);
typedef void(*spi_callback_t)(SPI_SLAVE_CALLBACK,uintptr_t);
typedef size_t(*spi_error_t)(void);
typedef bool(*spi_getflag_t)(void);
typedef void(*spi_nullary_t)(void);

extern "C"
char errstr[1024];


#pragma pack(push,1)
struct spi_pkt_header_t
{
    uint8_t start_byte;
    uint8_t size;
};
struct spi_pkt_footer_t
{
    uint8_t stop_byte;
};
#pragma pack(pop)

template<template<typename> typename tAllocator>
using spi_pkt_buffer_t = 
  std::deque
    < std::vector
       < uint8_t
       , tAllocator<uint8_t>
       >
    , tAllocator
       < std::vector
           < uint8_t
           , tAllocator<uint8_t>
           >
       >
    >;

static constexpr size_t max_packet_size = 256;
static constexpr size_t max_incoming_packets = 16;
static constexpr size_t max_outgoing_packets = 16;
    

template
  < spi_op_t tRead
  , spi_op_t tWrite
  , template<typename>
    typename tAllocator
  >
class spi_pkt_slave
{
    static constexpr uint8_t start_byte = 0xaa;
    static constexpr uint8_t stop_byte = 0x55;

    static TX_SEMAPHORE sem_incoming;
    static spi_pkt_buffer_t<tAllocator> *incoming;
    
    static TX_SEMAPHORE sem_outgoing;
    static spi_pkt_buffer_t<tAllocator> *outgoing;
    
    static TX_SEMAPHORE sem_read_api;
    static TX_THREAD proc_read;
    
    static TX_SEMAPHORE sem_write_api;
    static TX_THREAD proc_write;
    
    static uint8_t rbuf[max_packet_size];
    static uint8_t wbuf[max_packet_size];
    
    static constexpr size_t calc_dgram_size(size_t data_size)
    { return sizeof(spi_pkt_header_t)+data_size+sizeof(spi_pkt_footer_t);  }
    
        
    static void read_process(ULONG _unused)
    {
        tx_semaphore_ceiling_put(&sem_incoming,1);
        
        size_t packet_count = 0;
        while(true)
        {
            uint8_t *rbp = rbuf;
            size_t size = tRead(rbp,max_packet_size);

            while(size >= calc_dgram_size(0))
            {
                if(*rbp != start_byte)
                {
                    ++rbp; --size;
                    continue;
                }

                spi_pkt_header_t *pkt = (spi_pkt_header_t*)rbp;

                const size_t dgram_size = calc_dgram_size(pkt->size);

                if(dgram_size > size)
                {
                    ++rbp;
                    --size;
                    continue;
                }

                if(rbp[dgram_size-1] != stop_byte)
                {
                    ++rbp;
                    --size;
                    continue;
                }

                ++packet_count;

                tx_semaphore_get(&sem_incoming,TX_WAIT_FOREVER);

                static_assert(max_incoming_packets > 0, "max_incoming_packets must be at least 1");
                
                while(incoming->size() >= max_incoming_packets)
                    incoming->pop_front();

                incoming->emplace_back(rbp+sizeof(spi_pkt_header_t),rbp+dgram_size-1);
                
                tx_semaphore_ceiling_put(&sem_incoming,1);
                
                tx_semaphore_ceiling_put(&sem_read_api,1);

                size -= dgram_size;
            }
        }
        tx_semaphore_get(&sem_incoming,TX_WAIT_FOREVER);
    }
    
    static void write_process(ULONG _unused)
    {
        tx_semaphore_ceiling_put(&sem_outgoing,1);
        
        size_t packet_count = 0;
        while(true)
        {
            tx_semaphore_get(&sem_outgoing,TX_WAIT_FOREVER);
            if(!outgoing->size())
            {
                tx_semaphore_ceiling_put(&sem_outgoing,1);
                tx_semaphore_get(&sem_write_api,TX_WAIT_FOREVER);
                continue;
            }
            
            std::vector<uint8_t,tAllocator<uint8_t>> data = std::move(outgoing->front());
            outgoing->pop_front();
            
            tx_semaphore_ceiling_put(&sem_outgoing,1);
            
            const size_t packet_size = calc_dgram_size(data.size());
            assert(packet_size <= max_packet_size);
            
            spi_pkt_header_t *pkt = (spi_pkt_header_t*)wbuf;
            pkt->start_byte = start_byte;
            pkt->size = data.size();
            memcpy((void*)&pkt[1], data.data(), data.size());
            
            wbuf[packet_size-1] = stop_byte;
            
            ++packet_count;
            size_t size = tWrite(wbuf,packet_size);
            if(size != packet_size)
            {
                tx_semaphore_get(&sem_outgoing,TX_WAIT_FOREVER);
                if(!outgoing->size() < max_outgoing_packets)
                    outgoing->emplace_front(std::move(data));
                tx_semaphore_ceiling_put(&sem_outgoing,1);
            }
        }
        tx_semaphore_get(&sem_outgoing,TX_WAIT_FOREVER);
    }
    
public:

    static bool init()
    {
        tx_semaphore_create(&sem_read_api,"",0);
        tx_semaphore_create(&sem_write_api,"",0);
        tx_semaphore_create(&sem_incoming,"",0);
        tx_semaphore_create(&sem_outgoing,"",0);
        
        incoming = new spi_pkt_buffer_t<tAllocator>();
        outgoing = new spi_pkt_buffer_t<tAllocator>();
        
        auto res = tx_thread_create(&proc_read,"hostio-read",&read_process,0,(void*)new char[4096],4096, 1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);
        if(res != TX_SUCCESS)
        {
            __builtin_software_breakpoint();
            return false;
        }
       
        res = tx_thread_create(&proc_write,"hostio-write",&write_process,0,(void*)new long[4096],4096, 1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);
        if(res != TX_SUCCESS)
        {
            debug_long0 = res;
            __builtin_software_breakpoint();
            return false;
        }
        
        return true;
    }
    
    static std::vector<uint8_t,tAllocator<uint8_t>> recv(uint8_t &address)
    {
        while(true)
        {
            tx_semaphore_get(&sem_read_api,TX_WAIT_FOREVER);
            tx_semaphore_get(&sem_incoming,TX_WAIT_FOREVER);
            if(incoming->size())
                break;
            tx_semaphore_ceiling_put(&sem_incoming,1);
        }
        
        std::vector<uint8_t,tAllocator<uint8_t>> rv = std::move(incoming->front());
        incoming->pop_front();
        if(incoming->size())
            tx_semaphore_ceiling_put(&sem_read_api,1);
        tx_semaphore_ceiling_put(&sem_incoming,1);
        address = 0;
        return rv;
    }
    
    static bool send(uint8_t address, std::vector<uint8_t,tAllocator<uint8_t>> data)
    {
        if(data.size() > (max_packet_size-sizeof(spi_pkt_header_t)-sizeof(spi_pkt_footer_t)))
            return false;
        tx_semaphore_get(&sem_outgoing,TX_WAIT_FOREVER);
        if(outgoing->size() >= max_outgoing_packets)
            outgoing->pop_front();
        outgoing->emplace_back(std::move(data));
        tx_semaphore_ceiling_put(&sem_outgoing,1);
        tx_semaphore_ceiling_put(&sem_write_api,1);
        return true;
    }
};

#define MEMBER(_MA_type, _MA_name) \
template\
  < spi_op_t tRead\
  , spi_op_t tWrite\
  , template<typename>\
    typename tAllocator\
  >\
  _MA_type spi_pkt_slave<tRead,tWrite,tAllocator>::_MA_name

MEMBER(TX_SEMAPHORE,sem_incoming);
MEMBER(spi_pkt_buffer_t<tAllocator>*,incoming);

MEMBER(TX_SEMAPHORE,sem_outgoing);
MEMBER(spi_pkt_buffer_t<tAllocator>*,outgoing);

MEMBER(TX_SEMAPHORE,sem_read_api);
MEMBER(TX_THREAD,proc_read);

MEMBER(TX_SEMAPHORE,sem_write_api);
MEMBER(TX_THREAD,proc_write);

MEMBER(uint8_t,rbuf[max_packet_size]);
MEMBER(uint8_t,wbuf[max_packet_size]);

#undef MEMBER
}
#endif /* _EXAMPLE_FILE_NAME_H */

/* *****************************************************************************
 End of File
 */
