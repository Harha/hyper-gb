#ifndef JOYPAD_H
#define JOYPAD_H

#include "mem/memory_area.h"

namespace hgb
{

#define IO_REG_P1	0xFF00	// joypad (R/W)

class Joypad : public MemoryArea
{
public:
	Joypad();

	virtual byte read(word addr) override;
	virtual void write(word addr, byte value) override;
private:
	byte P1;
};

}

#endif // JOYPAD_H