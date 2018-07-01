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

byte RAM::getByte(word addr)
{
	return m_memory[map(addr)];
}

void RAM::setByte(word addr, byte value)
{
	m_memory[map(addr)] = value;
}

}