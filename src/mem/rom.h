#ifndef ROM_H
#define ROM_H

#include "mem/memory_area.h"

namespace hgb
{

class ROM : public MemoryArea
{
public:
	ROM(
		word address = 0x0000,
		size_t size = 0x4000
	);

	virtual byte read(word addr) override;
	virtual void write(word addr, byte value) override;
};

}

#endif // ROM_H