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
	throw std::runtime_error("ROM::setByte(...), error! Tried to modify read-only memory!");
}

}