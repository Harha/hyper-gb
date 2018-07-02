#ifndef MEMORY_AREA_H
#define MEMORY_AREA_H

#include "data_types.h"

namespace hgb
{

class MemoryArea
{
public:
	MemoryArea(
		word address = 0x0000,
		size_t size = 0x8000
	);
	virtual ~MemoryArea();

	// Get a true pointer to target data inside the memory area
	virtual word map(word addr);
	// Get a byte from the MemoryArea at specified 16-bit address
	virtual byte read(word addr) = 0;
	// Set a byte in the MemoryArea at specified 16-bit address
	virtual void write(word addr, byte value) = 0;

	virtual word getAddress();
	virtual size_t getSize();
	virtual byte * getMemory();
protected:
	word m_address;
	size_t m_size;
	byte * m_memory;
};

}

#endif // MEMORY_AREA_H