#include "mmu.h"
#include <stdexcept>
#include <cstdio>
#include "3rdparty/mlibc_log.h"
#include "bootrom.h"
#include "memory_area.h"
#include "rom.h"
#include "ram.h"

namespace hgb
{

MMU::MMU() :
	m_bootrom(nullptr),
	m_rom(),
	m_vram(nullptr),
	m_ram(),
	m_io(nullptr),
	m_hram(nullptr)
{
	// Init boot ROM
	m_bootrom = new ROM(0x0000, 0x0100);
	std::memcpy(m_bootrom->getMemory(), &BOOTROM_DMG01, BOOTROM_DMG01_SIZE);

	mlibc_dbg("MMU::MMU(...) Boot ROM, initialized bootstrap program!");

	// Init ROM bank(s)
	m_rom.push_back(new ROM(0x0000, 0x4000));
	m_rom.push_back(new ROM(0x4000, 0x4000));

	// Init VRAM
	m_vram = new RAM(0x8000, 0x2000);

	// Init RAM bank(s)
	m_ram.push_back(new RAM(0xC000, 0x2000));
	m_ram.push_back(new RAM(0xA000, 0x2000));

	// Init I/O
	m_io = new RAM(0xFF00, 0x004C);

	// Init HRAM
	m_hram = new RAM(0xFF80, 0x007F);

	mlibc_dbg("MMU::MMU(...)");
}

MMU::~MMU()
{
	// Free HRAM from memory
	delete m_hram;

	// Free I/O from memory
	delete m_io;

	// Free RAM banks from memory
	for (auto ma : m_ram)
	{
		delete ma;
	}

	// Free VRAM from memory
	delete m_vram;

	// Free ROM banks from memory
	for (auto ma : m_rom)
	{
		delete ma;
	}

	// Free boot ROM from memory
	delete m_bootrom;

	mlibc_dbg("MMU::~MMU(...)");
}

void MMU::loadROM(const std::string & fp)
{
	// Load file as binary + get file size
	FILE * file_ptr = fopen(fp.c_str(), "rb");
	fseek(file_ptr, 0, SEEK_END);
	long file_len = ftell(file_ptr);
	rewind(file_ptr);

	// Read the file into temp buffer and close it after
	byte * file_buf = new byte[file_len];
	fread(file_buf, file_len, 1, file_ptr);
	fclose(file_ptr);

	// Write the file contents into ROM
	for (long i = 0; i < file_len; i++)
	{
		// Get current address in 16-bit unsigned int
		word addr = static_cast<word>(i);

		// Choose the ROM bank to write to
		MemoryArea * rom = nullptr;
		if (addr >= MMU_ROM_BANK_0 && addr < MMU_ROM_BANK_X)
		{
			rom = m_rom[0];
		}
		else if (addr >= MMU_ROM_BANK_X && addr < MMU_VRAM)
		{
			rom = m_rom[1];
		}

		// If out of bounds, skip
		if (rom == nullptr)
			break;

		// Re-map the address
		addr = rom->map(addr);

		// Write the data
		rom->getMemory()[addr] = file_buf[i];
	}

	// Free the temp buffer
	delete file_buf;

	mlibc_dbg("MMU::loadROM(%s). file_len: %d", fp.c_str(), file_len);
}

MemoryArea * MMU::map(word addr)
{
	// ROM bank #0 & boot ROM
	if (addr >= MMU_ROM_BANK_0 && addr < MMU_ROM_BANK_X)
	{
		// Map to boot ROM if enabled
		if (addr <= MMU_ROM_BOOT_E && m_io->read(MMU_REG_BOOT) != 0x01)
			return m_bootrom;

		// Otherwise map to normal ROM
		return m_rom[0];
	}
	// ROM bank #x
	else if (addr >= MMU_ROM_BANK_X && addr < MMU_VRAM)
	{
		return m_rom[1];
	}
	// VRAM
	else if (addr >= MMU_VRAM && addr < MMU_RAM_BANK_X)
	{
		return m_vram;
	}
	// RAM bank #x
	else if (addr >= MMU_RAM_BANK_X && addr < MMU_RAM_BANK_0)
	{
		return m_ram[1];
	}
	// RAM bank #0
	else if (addr >= MMU_RAM_BANK_0 && addr < MMU_RAM_BANK_E)
	{
		return m_ram[0];
	}
	// I/O
	else if (addr >= MMU_IO && addr < MMU_EMPTY_1)
	{
		return m_io;
	}
	// HRAM
	else if (addr >= MMU_HRAM_S && addr <= MMU_HRAM_E)
	{
		return m_hram;
	}

	return nullptr;
}

byte MMU::read(word addr)
{
	// Get memory area mapped to this address
	MemoryArea * memory_area = map(addr);

	// Check if mapped memory_area is non-existent
	if (memory_area == nullptr)
	{
		//mlibc_wrn("MMU::read(...), warning! Tried to read from 0x%04zx which maps to a nullptr!", addr);
		return 0x00;
	}

	return memory_area->read(addr);
}

void MMU::write(word addr, byte value)
{
	// Get memory area mapped to this address
	MemoryArea * memory_area = map(addr);

	// Check if mapped memory_area is non-existent
	if (memory_area == nullptr)
	{
		//mlibc_wrn("MMU::write(...), warning! Tried to write to 0x%04zx which maps to a nullptr!", addr);
		return;
	}

	// Disallow writing to $FF50 after boot sequence
	if (addr == MMU_REG_BOOT && m_io->read(MMU_REG_BOOT) == 0x01)
	{
		mlibc_wrn("MMU::write(...), warning! Tried to write to $FF50 after bootstrap!");
		return;
	}

	memory_area->write(addr, value);
}

MemoryArea * MMU::getBootROM()
{
	return m_bootrom;
}

MemoryArea * MMU::getROM(size_t index)
{
	return m_rom[index];
}

MemoryArea * MMU::getVRAM()
{
	return m_vram;
}

MemoryArea * MMU::getRAM(size_t index)
{
	return m_ram[index];
}

MemoryArea * MMU::getIO()
{
	return m_io;
}

MemoryArea * MMU::getHRAM()
{
	return m_hram;
}

}