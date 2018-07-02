#include "ram.h"

namespace hgb
{

RAM::RAM(
	word address,
	size_t size
) :
	MemoryArea(
		address,
		size
	)
{

}

byte RAM::read(word addr)
{
	return m_memory[map(addr)];
}

void RAM::write(word addr, byte value)
{
	m_memory[map(addr)] = value;
}

}