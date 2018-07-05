#ifndef IRQ_H
#define IRQ_H

#include "mem/memory_area.h"

namespace hgb
{

#define IRQ_REG_IF	0xFF0F	// interrupt flags (R/W)
#define IRQ_REG_IE	0xFFFF	// interrupt enable (R/W)

class IRQ : public MemoryArea
{
public:
	IRQ();

	virtual byte read(word addr) override;
	virtual void write(word addr, byte value) override;
private:
	byte IF;
	byte IE;
};

}

#endif // IRQ_h