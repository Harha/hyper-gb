#ifndef MMU_H
#define MMU_H

#include <vector>
#include "data_types.h"

namespace hgb
{

// Memory area addresses (16-bit hex)
#define MMU_ROM_BOOT_S	0x0000	// boot rom start
#define MMU_ROM_BOOT_E	0x00FF	// boot rom end
#define MMU_ROM_BANK_0	0x0000	// rom bank #0
#define MMU_ROM_BANK_X	0x4000	// rom bank #x (switchable)
#define MMU_VRAM		0x8000	// vram
#define MMU_RAM_BANK_X	0xA000	// ram bank #x (switchable)
#define MMU_RAM_BANK_0	0xC000	// ram bank #0
#define MMU_RAM_BANK_E	0xE000	// ram bank #e (echoed ram bank #0)
#define MMU_SPR_ATTRIB	0xFE00	// sprite attrib memory
#define MMU_EMPTY_0		0xFEA0	// empty, unusable for i/o
#define MMU_IO			0xFF00	// i/o ports
#define MMU_EMPTY_1		0xFF4C	// empty, unusable for i/o
#define MMU_HRAM_S		0xFF80	// zero page start, 127 bytes
#define MMU_HRAM_E		0xFFFE	// zero page end

// Memory area registers (16-bit hex)
#define MMU_REG_BOOT	0xFF50	// enable/disable boot rom
#define MMU_REG_IME		0xFFFF	// interrupt enable flag

class MemoryArea;

class MMU
{
public:
	MMU();
	~MMU();

	// Load a ROM file
	void loadROM(const std::string & fp);
	// Get a true pointer to target memory area located around specified 16-bit address
	MemoryArea * map(word addr);
	// Get a byte at specified 16-bit address
	byte read(word addr);
	// Set a byte at specified 16-bit address
	void write(word addr, byte value);

	MemoryArea * getBootROM();
	MemoryArea * getROM(size_t index);
	MemoryArea * getVRAM();
	MemoryArea * getRAM(size_t index);
	MemoryArea * getIO();
	MemoryArea * getHRAM();
private:
	MemoryArea * m_bootrom;
	std::vector<MemoryArea *> m_rom;
	MemoryArea * m_vram;
	std::vector<MemoryArea *> m_ram;
	// todo: sprite attrib memory
	// todo: empty, unusable for i/o
	MemoryArea * m_io;
	MemoryArea * m_hram;
};

}

#endif // MMU_H