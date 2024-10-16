#ifndef DRIVER_MCP23S17_H
# define DRIVER_MCP23S17_H
# include <cstddef>
# include <mutex>
# include <drivers/spi.hh>


class MCP23S17
{
	SPI &spi;
	GPIO_PIN pin_cs;
	GPIO_PIN pin_reset;

	std::mutex _value_lk;
	volatile uint16_t _value;

	void _value_write();

public:

	MCP23S17(SPI &s, GPIO_PIN cs, GPIO_PIN rst);

	uint16_t value();

	void value(uint16_t pins);

	bool pin_value(uint8_t pin);

	void pin_value(uint8_t pin, bool value);
};
#endif
