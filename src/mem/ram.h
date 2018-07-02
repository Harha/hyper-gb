#ifndef RAM_H
#define RAM_H

#include "mem/memory_area.h"

namespace hgb
{

class RAM : public MemoryArea
{
public:
	RAM(
		word address = 0xC000,
		size_t size = 0x2000
	);

	virtual byte read(word addr) override;
	virtual void write(word addr, byte value) override;
};

}

#endif // RAM_H