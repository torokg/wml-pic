#ifndef DRIVERS_SPI_HH
# define DRIVERS_SPI_HH
# include <mutex>
# include "definitions.h"
struct SPI
{
	bool(*const Read)(void* dest, size_t size);
	bool(*const Write)(void* src, size_t size);
	bool(*const WriteRead)(void* src, size_t wsize, void *dst, size_t rsize);
    std::mutex mutex;
};

#endif