#include "irq.h"

namespace hgb
{

IRQ::IRQ() :
	MemoryArea(
		0xFF0F,
		0x0000
	),
	IF(),
	IE()
{

}

byte IRQ::read(word addr)
{
	if (addr == IRQ_REG_IF)
		return IF;
	else if (addr == IRQ_REG_IE)
		return IE;

	return 0x00;
}

void IRQ::write(word addr, byte value)
{
	if (addr == IRQ_REG_IF)
		IF = value;
	else if (addr == IRQ_REG_IE)
		IE = value;
}

}