/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.h

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

#ifndef IO_HOST_SPI_PKT_SLAVE_HH    /* Guard against multiple inclusion */
#define IO_HOST_SPI_PKT_SLAVE_HH

#include <cassert>
#include <cstring>
#include <limits>
#include <deque>
#include <vector>

#include "definitions.h"
#include <peripheral/spi/spi_slave/plib_spi_slave_common.h>


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
  , spi_callback_t tCallback
  , spi_error_t tError
  , spi_getflag_t tIsBusy
  , spi_getflag_t tWriteComplete
  , spi_nullary_t tClkRqSet
  , spi_nullary_t tClkRqClr
  , template<typename>
    typename tAllocator
  >
class spi_pkt_slave
{
    static SemaphoreHandle_t sem_incoming;
    static spi_pkt_buffer_t<tAllocator> *incoming;
    
    static SemaphoreHandle_t sem_outgoing;
    static spi_pkt_buffer_t<tAllocator> *outgoing;
    
    static SemaphoreHandle_t sem_read;
    static SemaphoreHandle_t sem_read_api;
    static TaskHandle_t proc_read;
    
    static SemaphoreHandle_t sem_write;
    static SemaphoreHandle_t sem_write_api;
    static TaskHandle_t proc_write;

    static constexpr uint8_t start_byte = 0xaa;
    static constexpr uint8_t stop_byte = 0x55;
    
    static constexpr size_t calc_dgram_size(size_t data_size)
    { return sizeof(spi_pkt_header_t)+data_size+sizeof(spi_pkt_footer_t);  }
    
    static volatile size_t read_size;

    static volatile bool read_block;
    
    static volatile bool write_success;
   
    static uint8_t rbuf[2][max_packet_size];
    static uint8_t wbuf[max_packet_size];
    
    static void callback(uintptr_t, uintptr_t reason)
    {
        auto xHigherPriorityTaskWoken = pdTRUE;
        switch(reason)
        {
            case 0: // SPI_TRANSFER_STARTED
                write_success = true;
                break;
              
            case 1: // SPI_TRANSFER_FINISHED
            {
                if(!read_block)
                {
                    read_size = tRead(rbuf[0],max_packet_size);
                    xSemaphoreGiveFromISR(sem_read, &xHigherPriorityTaskWoken);
                }
                if(tWriteComplete())
                {
                    xHigherPriorityTaskWoken = pdTRUE;
                    xSemaphoreGiveFromISR(sem_write, &xHigherPriorityTaskWoken);
                }
                break;
            }
            
            case 2: // SPI_TRANSMISSION_FAILED
                write_success = false;
                xSemaphoreGiveFromISR(sem_write, &xHigherPriorityTaskWoken);
                break;
        }
    }
        
    static void read_process()
    {
        spi_pkt_buffer_t<tAllocator> icp;
        incoming = &icp;
        xSemaphoreGive(sem_incoming); 
        
        size_t packet_count = 0;
        while(true)
        {
            while(!xSemaphoreTake(sem_read,portMAX_DELAY));
         
            read_block = true;
            size_t size = read_size;
            memcpy(rbuf[1],rbuf[0],size);
            memset(rbuf[0],0,max_packet_size);
            read_block = false;
            
            if(!size || (size > max_packet_size))
                continue;
           
            uint8_t *rbp = rbuf[1];

            while(size)
            {
                while(size && (*rbp != start_byte))
                { ++rbp; --size; }

                if(size < calc_dgram_size(0))
                    break;

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

                while(!xSemaphoreTake(sem_incoming, portMAX_DELAY));

                static_assert(max_incoming_packets > 0, "max_incoming_packets must be at least 1");
                while(incoming->size() >= max_incoming_packets)
                    incoming->pop_front();

                incoming->emplace_back(rbp+sizeof(spi_pkt_header_t),rbp+dgram_size-1);
                
                xSemaphoreGive(sem_incoming);
                xSemaphoreGive(sem_read_api);

                size -= dgram_size;
            }
        }
        while(!xSemaphoreTake(sem_incoming,portMAX_DELAY));
        incoming = 0;
    }
    
    static void write_process()
    {
        spi_pkt_buffer_t<tAllocator> ogp;
        outgoing = &ogp;
        xSemaphoreGive(sem_outgoing);
        
        size_t packet_count = 0;
        while(true)
        {
            while(!xSemaphoreTake(sem_outgoing, portMAX_DELAY));
            if(!outgoing->size())
            {
                xSemaphoreGive(sem_outgoing);
                while(!xSemaphoreTake(sem_write_api,portMAX_DELAY));
                continue;
            }
            
            std::vector<uint8_t,tAllocator<uint8_t>> data = std::move(outgoing->front());
            outgoing->pop_front();
            
            xSemaphoreGive(sem_outgoing);
            
            const size_t packet_size = calc_dgram_size(data.size());
            assert(packet_size <= max_packet_size);
            
            spi_pkt_header_t *pkt = (spi_pkt_header_t*)wbuf;
            pkt->start_byte = start_byte;
            pkt->size = data.size();
            memcpy((void*)&pkt[1], data.data(), data.size());
            
            wbuf[packet_size-1] = stop_byte;
            
            ++packet_count;
            xSemaphoreTake(sem_write,0);
            tWrite(wbuf,packet_size);
            tClkRqSet();
            while(!xSemaphoreTake(sem_write,portMAX_DELAY));
            tClkRqClr();
            if(!write_success)
            {
                while(!xSemaphoreTake(sem_outgoing, portMAX_DELAY));
                outgoing->emplace_front(std::move(data));
                xSemaphoreGive(sem_outgoing);
            }
        }
        while(!xSemaphoreTake(sem_outgoing,portMAX_DELAY));
        outgoing = 0;
    }
    
public:

    static bool init()
    {
        sem_read = xSemaphoreCreateBinary();
        sem_read_api = xSemaphoreCreateBinary();
        
        sem_write = xSemaphoreCreateBinary();
        sem_write_api = xSemaphoreCreateBinary();
        
        read_size = 0;
        
        sem_incoming = xSemaphoreCreateBinary();
        sem_outgoing = xSemaphoreCreateBinary();

        if(xTaskCreate((TaskFunction_t)&read_process,"hostio-read",4096,(void*)0,1,&proc_read) != pdPASS)
            return false;
        
        if(xTaskCreate((TaskFunction_t)&write_process,"hostio-write",4096,(void*)0,1,&proc_write) != pdPASS)
            return false;
        
        tCallback(callback,0);
        
        return true;
    }
    
    static std::vector<uint8_t,tAllocator<uint8_t>> recv(uint8_t &address)
    {
        while(true)
        {
            while(!xSemaphoreTake(sem_read_api, portMAX_DELAY));
            while(!xSemaphoreTake(sem_incoming, portMAX_DELAY));
            if(incoming->size())
                break;
            xSemaphoreGive(sem_incoming);
        }
        
        std::vector<uint8_t,tAllocator<uint8_t>> rv = std::move(incoming->front());
        incoming->pop_front();
        if(incoming->size())
            xSemaphoreGive(sem_read_api);
        xSemaphoreGive(sem_incoming);
	address = 0;
        return rv;
    }
    
    static bool send(uint8_t address, std::vector<uint8_t,tAllocator<uint8_t>> data)
    {
        if(data.size() > (max_packet_size-sizeof(spi_pkt_header_t)-sizeof(spi_pkt_footer_t)))
            return false;
        while(!xSemaphoreTake(sem_outgoing, portMAX_DELAY));
        if(outgoing->size() >= max_outgoing_packets)
            outgoing->pop_front();
        outgoing->emplace_back(std::move(data));
        xSemaphoreGive(sem_outgoing);
        xSemaphoreGive(sem_write_api);
        return true;
    }
};

#define MEMBER(_MA_type, _MA_name) \
template\
  < spi_op_t tRead\
  , spi_op_t tWrite\
  , spi_callback_t tCallback\
  , spi_error_t tError\
  , spi_getflag_t tIsBusy\
  , spi_getflag_t tWriteComplete\
  , spi_nullary_t tClkRqSet\
  , spi_nullary_t tClkRqClr\
  , template<typename>\
    typename tAllocator\
  >\
  _MA_type spi_pkt_slave<tRead,tWrite,tCallback,tError,tIsBusy,tWriteComplete,tClkRqSet,tClkRqClr,tAllocator>::_MA_name

MEMBER(SemaphoreHandle_t,sem_incoming);
MEMBER(spi_pkt_buffer_t<tAllocator>*,incoming);

MEMBER(SemaphoreHandle_t,sem_outgoing);
MEMBER(spi_pkt_buffer_t<tAllocator>*,outgoing);

MEMBER(SemaphoreHandle_t,sem_read);
MEMBER(SemaphoreHandle_t,sem_read_api);
MEMBER(TaskHandle_t,proc_read);

MEMBER(SemaphoreHandle_t,sem_write);
MEMBER(SemaphoreHandle_t,sem_write_api);
MEMBER(TaskHandle_t,proc_write);

MEMBER(uint8_t,rbuf[2][max_packet_size]);
MEMBER(uint8_t,wbuf[max_packet_size]);
MEMBER(volatile size_t,read_size) = 0;

MEMBER(volatile bool,read_block) = false;
MEMBER(volatile bool,write_success) = false;


#undef MEMBER
}
#endif /* _EXAMPLE_FILE_NAME_H */

/* *****************************************************************************
 End of File
 */
