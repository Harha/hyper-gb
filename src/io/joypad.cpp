#include "joypad.h"

namespace hgb
{

Joypad::Joypad() :
	MemoryArea(
		0xFF00,
		0x0000
	),
	P1()
{

}

byte Joypad::read(word addr)
{
	if (addr == IO_REG_P1)
		return P1;

	return 0x00;
}

void Joypad::write(word addr, byte value)
{
	if (addr == IO_REG_P1)
		P1 = value;
}

}