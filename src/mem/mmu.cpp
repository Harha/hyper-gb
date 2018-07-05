#include "mmu.h"
#include <stdexcept>
#include <cstdio>
#include "3rdparty/mlibc_log.h"
#include "mem/bootrom.h"
#include "mem/memory_area.h"
#include "mem/rom.h"
#include "mem/ram.h"
#include "cpu/irq.h"
#include "io/joypad.h"
#include "io/timer.h"
#include "ppu/ppu.h"

namespace hgb
{

MMU::MMU(
	MemoryArea & irq,
	MemoryArea & joy,
	MemoryArea & timer,
	MemoryArea & ppu
) :
	m_cart(nullptr),
	m_bootrom(nullptr),
	m_rom(),
	m_vram(nullptr),
	m_ram(),
	m_irq(irq),
	m_joy(joy),
	m_timer(timer),
	m_ppu(ppu),
	m_ff50(),
	m_hram(nullptr)
{
	// Init boot ROM
	m_bootrom = new ROM(0x0000, 0x0100);
	std::memcpy(m_bootrom->getMemory(), &BOOTROM_DMG01, BOOTROM_DMG01_SIZE);

	mlibc_dbg("MMU::MMU(...) Boot ROM, initialized bootstrap program!");

	// Init ROM bank(s)
	m_rom.push_back(new ROM(MMU_ROM_BANK_0, MMU_ROM_BANK_SZ));
	m_rom.push_back(new ROM(MMU_ROM_BANK_X, MMU_ROM_BANK_SZ));

	// Init VRAM
	m_vram = new RAM(MMU_VRAM, MMU_VRAM_SZ);

	// Init RAM bank(s)
	m_ram.push_back(new RAM(MMU_RAM_BANK_0, MMU_RAM_BANK_SZ));
	m_ram.push_back(new RAM(MMU_RAM_BANK_X, MMU_RAM_BANK_SZ));

	// Init other registers
	m_ff50 = 0x00;

	// Init HRAM
	m_hram = new RAM(MMU_HRAM, MMU_HRAM_SZ);

	mlibc_dbg("MMU::MMU(...)");
}

MMU::~MMU()
{
	// Free HRAM from memory
	delete m_hram;

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

	// Free cartridge data
	delete m_cart;

	mlibc_dbg("MMU::~MMU(...)");
}

void MMU::loadROM(const std::string & fp)
{
	// Init new cartridge instance
	m_cart = new Cartridge;

	// Load file as binary + get file size
	FILE * file_ptr = fopen(fp.c_str(), "rb");

	if (file_ptr == NULL)
		throw std::runtime_error("MMU::loadROM(...), error! fopen returned a NULL pointer!");

	fseek(file_ptr, 0, SEEK_END);
	m_cart->data_len = ftell(file_ptr);
	rewind(file_ptr);

	// Read the file into cartridge data and close it after
	m_cart->data = new byte[m_cart->data_len];
	fread(m_cart->data, m_cart->data_len, 1, file_ptr);
	fclose(file_ptr);

	// Read cartridge metadata
	m_cart->game_title = std::string(&m_cart->data[CRT_GAME_TITLE_S], &m_cart->data[CRT_GAME_TITLE_S] + CRT_GAME_TITLE_SZ);
	m_cart->manuf_code = std::string(&m_cart->data[CRT_MANUF_CODE_S], &m_cart->data[CRT_MANUF_CODE_S] + CRT_MANUF_CODE_SZ);
	m_cart->gbc_flag = m_cart->data[CRT_GBC_FLAG];
	m_cart->license_code_new = std::string(&m_cart->data[CRT_LICENSE_CODE_NEW_S], &m_cart->data[CRT_LICENSE_CODE_NEW_S] + CRT_LICENSE_CODE_NEW_SZ);
	m_cart->sgb_flag = m_cart->data[CRT_SGB_FLAG];
	m_cart->type = m_cart->data[CRT_TYPE];
	m_cart->rom_size = m_cart->data[CRT_ROM_SIZE];
	m_cart->ram_size = m_cart->data[CRT_RAM_SIZE];
	m_cart->destination_code = m_cart->data[CRT_DEST_CODE];
	m_cart->license_code_old = m_cart->data[CRT_LICENSE_CODE_OLD];
	m_cart->rom_version = m_cart->data[CRT_ROM_VERSION];
	m_cart->header_checksum = m_cart->data[CRT_HEADER_CHECKSUM];
	m_cart->global_checksum = word_(m_cart->data[CRT_GLOBAL_CHECKSUM_E], m_cart->data[CRT_GLOBAL_CHECKSUM_S]);

	// Print cartridge metadata
	mlibc_inf("game_title: %s", m_cart->game_title.c_str());
	mlibc_inf("manuf_code: %s", m_cart->manuf_code.c_str());
	mlibc_inf("gbc_flag: 0x%02zx", m_cart->gbc_flag);
	mlibc_inf("license_code_new: %s", m_cart->license_code_new.c_str());
	mlibc_inf("sgb_flag: 0x%02zx", m_cart->sgb_flag);
	mlibc_inf("type: 0x%02zx", m_cart->type);
	mlibc_inf("rom_size: 0x%02zx", m_cart->rom_size);
	mlibc_inf("ram_size: 0x%02zx", m_cart->ram_size);
	mlibc_inf("desination_code: 0x%02zx", m_cart->destination_code);
	mlibc_inf("license_code_old: 0x%02zx", m_cart->license_code_old);
	mlibc_inf("rom_version: 0x%02zx", m_cart->rom_version);
	mlibc_inf("header_checksum: 0x%02zx", m_cart->header_checksum);
	mlibc_inf("global_checksum: 0x%04zx", m_cart->global_checksum);

	// Write the file contents into ROM
	for (long i = 0; i < m_cart->data_len; i++)
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
		{
			mlibc_err("MMU::loadROM(%s). file_len exceeds 0x8000, cannot load ROM properly!", fp.c_str());
			break;
		}

		// Re-map the address
		addr = rom->map(addr);

		// Write the data
		rom->getMemory()[addr] = m_cart->data[i];
	}

	mlibc_dbg("MMU::loadROM(%s). data_len: %d", fp.c_str(), m_cart->data_len);
}

MemoryArea * MMU::map(word addr)
{
	// ROM bank #0 & boot ROM
	if (addr >= MMU_ROM_BANK_0 && addr < MMU_ROM_BANK_X)
	{
		// Map to boot ROM if enabled
		if (addr <= MMU_ROM_BOOT_E && m_ff50 != 0x01)
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
	// IRQ ctrl
	else if (addr == IRQ_REG_IF || addr == IRQ_REG_IE)
	{
		return &m_irq;
	}
	// Joypad ctrl
	else if (addr == IO_REG_P1)
	{
		return &m_joy;
	}
	// Timer
	else if (addr >= TIMER_REG_S && addr <= TIMER_REG_E)
	{
		return &m_timer;
	}
	// PPU
	else if (addr >= PPU_REG_S && addr <= PPU_REG_E)
	{
		return &m_ppu;
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

	// Check if mapped memory area is non-existent
	if (memory_area == nullptr)
	{
		// enable / disable bootrom register
		if (addr == MMU_REG_BOOT)
			return m_ff50;

		return 0x00;
	}

	return memory_area->read(addr);
}

void MMU::write(word addr, byte value)
{
	// Get memory area mapped to this address
	MemoryArea * memory_area = map(addr);

	// Check if mapped memory area is non-existent
	if (memory_area == nullptr)
	{
		// enable / disable bootrom register (allow writing to only once!)
		if (addr == MMU_REG_BOOT && m_ff50 == 0x00)
			m_ff50 = value;

		return;
	}

	memory_area->write(addr, value);
}

Cartridge * MMU::getCart()
{
	return m_cart;
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

MemoryArea & MMU::getIRQ()
{
	return m_irq;
}

MemoryArea & MMU::getJoypad()
{
	return m_joy;
}

MemoryArea & MMU::getTimer()
{
	return m_timer;
}

MemoryArea & MMU::getPPU()
{
	return m_ppu;;
}

byte & MMU::getFF50()
{
	return m_ff50;
}

MemoryArea * MMU::getHRAM()
{
	return m_hram;
}

}