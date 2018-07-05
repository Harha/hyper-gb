#include "timer.h"

namespace hgb
{

Timer::Timer() :
	MemoryArea(
		0xFF04,
		0x0000
	),
	DIV(),
	TIMA(),
	TMA(),
	TAC()
{

}

byte Timer::read(word addr)
{
	if (addr == TIMER_REG_DIV)
		return DIV;
	else if (addr == TIMER_REG_TIMA)
		return TIMA;
	else if (addr == TIMER_REG_TMA)
		return TMA;
	else if (addr == TIMER_REG_TAC)
		return TAC;

	return 0x00;
}

void Timer::write(word addr, byte value)
{
	if (addr == TIMER_REG_DIV)
		DIV = value;
	else if (addr == TIMER_REG_TIMA)
		TIMA = value;
	else if (addr == TIMER_REG_TMA)
		TMA = value;
	else if (addr == TIMER_REG_TAC)
		TAC = value;
}

}