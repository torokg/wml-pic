#include <thread>
#include "../MCP23S17.h"


MCP23S17::MCP23S17(SPI &s, GPIO_PIN cs, GPIO_PIN rst)
	: spi(s)
	, pin_cs(cs)
	, pin_reset(rst)
	, _value(0)

{
	GPIO_PinWrite(pin_reset,1);
	GPIO_PinWrite(pin_cs,1);
	
	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	static uint8_t all_output[4] = {0b01000000,0x0,0b00000000,0b00000000};
	
	GPIO_PinWrite(pin_cs,0);
	spi.Write(all_output,sizeof(all_output));
	GPIO_PinWrite(pin_cs,1);
}

uint16_t
MCP23S17::value()
{
	std::unique_lock ul(_value_lk);
	return _value;
}

void
MCP23S17::_value_write()
{
	uint8_t wbuf[4] = {0b01000000,0x14,reinterpret_cast<volatile uint8_t*>(&_value)[0],reinterpret_cast<volatile uint8_t*>(&_value)[1]};
	GPIO_PinWrite(pin_cs,0);
	spi.Write(wbuf,sizeof(wbuf));
	GPIO_PinWrite(pin_cs,1);
}

void
MCP23S17::value(uint16_t pins)
{
	std::unique_lock lk(_value_lk);
	_value = pins;
	_value_write();
}
bool
MCP23S17::pin_value(uint8_t pin)
{
	std::unique_lock lk(_value_lk);
	return (_value>>pin)&1;
}

void
MCP23S17::pin_value(uint8_t pin, bool value)
{
	const auto mask = (1<<pin);
	std::unique_lock lk(_value_lk);
	if(value)
		_value |= mask;
	else
		_value &= ~mask;
	_value_write();
}
