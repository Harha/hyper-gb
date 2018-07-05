#ifndef TIMER_H
#define TIMER_H

#include "mem/memory_area.h"

namespace hgb
{

#define TIMER_REG_S		0xFF04	// timer memory area start
#define TIMER_REG_E		0xFF07	// timer memory area end
#define TIMER_REG_DIV	0xFF04	// divider register (R/W)
#define TIMER_REG_TIMA	0xFF05	// timer counter register (R/W)
#define TIMER_REG_TMA	0xFF06	// timer modulo register (R/W)
#define TIMER_REG_TAC	0xFF07	// timer control register (R/W)

class Timer : public MemoryArea
{
public:
	Timer();

	virtual byte read(word addr) override;
	virtual void write(word addr, byte value) override;
private:
	byte DIV;
	byte TIMA;
	byte TMA;
	byte TAC;
};

}

#endif // TIMER_H