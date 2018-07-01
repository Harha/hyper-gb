#include "memory_area.h"
#include <algorithm>
#include "3rd party/mlibc_log.h"

namespace hgb
{

MemoryArea::MemoryArea(
	word address,
	size_t size
) :
	m_address(address),
	m_size(size),
	m_memory(new byte[m_size])
{
	std::fill_n(m_memory, m_size, 0x00);

	mlibc_dbg("MemoryArea::MemoryArea(address:0x%04zx, size:%zu)", m_address, m_size);
}

MemoryArea::~MemoryArea()
{
	delete m_memory;

	mlibc_dbg("MemoryArea::~MemoryArea(), address: 0x%04zx, size: %zu", m_address, m_size);
}

word MemoryArea::getAddress()
{
	return m_address;
}

size_t MemoryArea::getSize()
{
	return m_size;
}

byte * MemoryArea::getMemory()
{
	return m_memory;
}

word MemoryArea::map(word addr)
{
	return addr - m_address;
}

}