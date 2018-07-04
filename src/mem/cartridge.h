#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <string>
#include "data_types.h"

namespace hgb
{

// Cart metadata addresses (16-bit hex)
#define CRT_GAME_TITLE_S				0x0134
#define CRT_GAME_TITLE_E				0x0143
#define CRT_GAME_TITLE_SZ				0x0048
#define CRT_MANUF_CODE_S				0x013F
#define CRT_MANUF_CODE_E				0x0142
#define CRT_MANUF_CODE_SZ				0x0004
#define CRT_GBC_FLAG					0x0143
#define CRT_LICENSE_CODE_NEW_S			0x0144
#define CRT_LICENSE_CODE_NEW_E			0x0145
#define CRT_LICENSE_CODE_NEW_SZ			0x0002
#define CRT_SGB_FLAG					0x0146
#define CRT_TYPE						0x0147
#define CRT_ROM_SIZE					0x0148
#define CRT_RAM_SIZE					0x0149
#define CRT_DEST_CODE					0x014A
#define CRT_LICENSE_CODE_OLD			0x014B
#define CRT_ROM_VERSION					0x014C
#define CRT_HEADER_CHECKSUM				0x014D
#define CRT_GLOBAL_CHECKSUM_S			0x014E
#define CRT_GLOBAL_CHECKSUM_E			0x014F
#define CRT_GLOBAL_CHECKSUM_SZ			0x0001

// GBC flag values
#define CRT_GBC_FLAG_GB					0x80
#define CRT_GBC_FLAG_GBC				0xC0

// SGB flag values
#define CRT_SGB_FLAG_NO					0x00
#define CRT_SGB_FLAG_YES				0x03

// Cartridge type values
#define CRT_TYPE_ROM_ONLY				0x00
#define CRT_TYPE_MBC1					0x01
#define CRT_TYPE_MBC1_RAM				0x02
#define CRT_TYPE_MBC1_RAM_BATTERY		0x03
#define CRT_TYPE_MBC2					0x05
#define CRT_TYPE_MBC2_RAM_BATTERY		0x06
#define CRT_TYPE_ROM_RAM				0x08
#define CRT_TYPE_ROM_RAM_BATTERY		0x09
#define CRT_TYPE_MMM01					0x0B
#define CRT_TYPE_MMM01_RAM				0x0C
#define CRT_TYPE_MMM01_RAM_BATTERY		0x0D
#define CRT_TYPE_MBC3_TIMER_BATTERY		0x0F
#define CRT_TYPE_MBC3_RAM_TIMER_BATTERY	0x010
#define CRT_TYPE_MBC3					0x11
#define CRT_TYPE_MBC3_RAM				0x12
#define CRT_TYPE_MBC3_RAM_BATTERY		0x13
// ... missing 0x14-0x22, 0xFC-0xFF

// ROM size values
#define CRT_ROM_SZ_32KB					0x00	// 2 banks (no MBC)
#define CRT_ROM_SZ_64KB					0x01	// 4 banks
#define CRT_ROM_SZ_128KB				0x02	// 8 banks
#define CRT_ROM_SZ_256KB				0x03	// 16 banks
#define CRT_ROM_SZ_512KB				0x04	// 32 banks
#define CRT_ROM_SZ_1MB					0x05	// 64 banks
#define CRT_ROM_SZ_2MB					0x06	// 128 banks
#define CRT_ROM_SZ_4MB					0x07	// 256 banks
#define CRT_ROM_SZ_8MB					0x08	// 512 banks

// RAM size values
#define CRT_RAM_SZ_NONE					0x00
#define CRT_RAM_SZ_2KB					0x01
#define CRT_RAM_SZ_8KB					0x02	// 1 bank
#define CRT_RAM_SZ_32KB					0x03	// 4 banks of 8KB
#define CRT_RAM_SZ_128KB				0x04	// 16 banks of 8KB
#define CRT_RAM_SZ_64KB					0x05	// 8 banks of 8KB

// Destination code values
#define CRT_DEST_CODE_JAPAN				0x00
#define CRT_DEST_CODE_UNIVERSAL			0x01

// Old licensee code values
#define CRT_LICENSE_CODE_OLD_USE_NEW	0x33

struct Cartridge
{
	// data + data length in bytes
	byte * data;
	long data_len;

	// Game title (0x0134-0x0143)
	std::string game_title;

	// Manufacturer code (0x013F-0x0142)
	std::string manuf_code;

	// GBC flag (0x0143)
	byte gbc_flag;

	// New licensee code (0x0144-0x0145)
	std::string license_code_new;

	// SGB flag (0x0146)
	byte sgb_flag;

	// Cartridge type (0x0147)
	byte type;

	// ROM size (0x0148)
	byte rom_size;

	// RAM size (0x0149)
	byte ram_size;

	// Destination code (0x014A)
	byte destination_code;

	// Old licensee code (0x014B)
	byte license_code_old;

	// ROM version (0x014C)
	byte rom_version;

	// Header checksum (0x014D)
	byte header_checksum;

	// Global checksum (0x014E-0x014F)
	word global_checksum;
};

}

#endif // CARTRIDGE_H