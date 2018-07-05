#include "rom.h"
#include <stdexcept>
#include <cstring>
#include "3rdparty/mlibc_log.h"

namespace hgb
{

ROM::ROM(
	word address,
	size_t size
) :
	MemoryArea(
		address,
		size
	)
{

}

byte ROM::read(word addr)
{
	return m_memory[map(addr)];
}

void ROM::write(word addr, byte value)
{
	mlibc_wrn("ROM::write(0x%04zx), error! Tried to modify read-only memory!", addr);
}

}