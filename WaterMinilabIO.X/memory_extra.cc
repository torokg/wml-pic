#include <cstdint>
#include <new>
#include <mutex>
#include "tx_api.h"

#define POSIX_ALLOCATION 1

#ifdef POSIX_ALLOCATION
extern "C" int posix_memory_allocate(unsigned long size, void **memory_ptr);
extern "C" void posix_memory_release(void *ptr);

static bool _alloc_ready = false;
extern "C" void alloc_initialize()
{ _alloc_ready = true; }

static constexpr size_t _overalloc = 0;

static inline void *_alloc(std::size_t count)
{
    if(!_alloc_ready)
        __builtin_software_breakpoint();
    
    void *rv = 0;
    posix_memory_allocate(count+_overalloc, &rv);
    rv += _overalloc/2;
    return rv;
}

static inline void _free(void *ptr)
{
    if(!_alloc_ready)
        __builtin_software_breakpoint();
    ptr -= _overalloc/2;
    posix_memory_release(ptr);
}

constexpr size_t _malloc_buffer_size = 8192;
static long _malloc_buffer[_malloc_buffer_size];
static volatile long _malloc_ptr = 0;

extern "C" void *malloc(size_t size)
{
    void *rv = 0;
    __builtin_disable_interrupts();
    size = (size + sizeof(long)-1)/sizeof(long)*sizeof(long);
    if(_malloc_ptr + size <= _malloc_buffer_size)
    {
        rv = reinterpret_cast<void*>
              (reinterpret_cast<uint8_t*>(_malloc_buffer)+_malloc_ptr);
        _malloc_ptr += size;
    }
    else
    {
        __builtin_software_breakpoint();
    }
    
    __builtin_enable_interrupts();
    return rv;
    //return _alloc(size);
}


extern "C" void free(void *p)
{
    
}

#else
extern "C" void alloc_initialize()
{}

static std::mutex *_alloc_lk = 0;

static inline void *_alloc(std::size_t count)
{
    if(!_alloc_lk)
    {
        _alloc_lk = (std::mutex*)malloc(sizeof(std::mutex));
        new(_alloc_lk) std::mutex();
    }
    std::unique_lock ul(*_alloc_lk);
    return malloc(count);
}

static inline void _free(void *ptr)
{
    std::unique_lock ul(*_alloc_lk);
    free(ptr);
}
#endif

////////////////////////////////////////////////////////////////////////////////
void* operator new(std::size_t count)
{ return _alloc(count); }

void* operator new[](std::size_t count)
{ return _alloc(count); }

void* operator new( std::size_t count, std::align_val_t al )
{ return _alloc(count); }

void* operator new[]( std::size_t count, std::align_val_t al )
{ return _alloc(count); }

void* operator new( std::size_t count, const std::nothrow_t& tag ) noexcept
{ return _alloc(count); }

void* operator new[]( std::size_t count, const std::nothrow_t& tag )  noexcept
{ return _alloc(count); }

void* operator new  ( std::size_t count, std::align_val_t al, const std::nothrow_t& tag ) noexcept
{ return _alloc(count); }

void* operator new[]( std::size_t count, std::align_val_t al, const std::nothrow_t& tag ) noexcept
{ return _alloc(count); }


void operator delete  ( void* ptr ) noexcept
{ _free(ptr); }

void operator delete[]( void* ptr ) noexcept
{ _free(ptr); }

void operator delete  ( void* ptr, std::align_val_t al ) noexcept
{ _free(ptr); }

void operator delete[]( void* ptr, std::align_val_t al ) noexcept
{ _free(ptr); }

void operator delete  ( void* ptr, std::size_t sz ) noexcept
{ _free(ptr); }

void operator delete[]( void* ptr, std::size_t sz ) noexcept
{ _free(ptr); }

void operator delete  ( void* ptr, std::size_t sz, std::align_val_t al ) noexcept
{ _free(ptr); }

void operator delete[]( void* ptr, std::size_t sz, std::align_val_t al ) noexcept
{ _free(ptr); }
