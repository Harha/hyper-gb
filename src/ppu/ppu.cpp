#include "ppu.h"
#include "3rdparty/mlibc_log.h"

namespace hgb
{

PPU::PPU() :
	MemoryArea(
		0xFF04,
		0x0000
	),
	LCDC(),
	STAT(),
	SCY(),
	SCX(),
	LY(),
	LYC(),
	DMA(),
	BGP(),
	OBP0(),
	OBP1(),
	WY(),
	WX()
{
	mlibc_dbg("PPU::PPU()");
}

PPU::~PPU()
{
	mlibc_dbg("PPU::~PPU()");
}

void PPU::tick()
{
	LY++;
}

byte PPU::read(word addr)
{
	switch (addr)
	{
		case PPU_REG_LCDC:
		{

		} break;
		case PPU_REG_STAT:
		{

		} break;
		case PPU_REG_SCY:
		{

		} break;
		case PPU_REG_SCX:
		{

		} break;
		case PPU_REG_LY:
		{
			return LY;
		} break;
		case PPU_REG_LYC:
		{

		} break;
		case PPU_REG_DMA:
		{

		} break;
		case PPU_REG_BGP:
		{

		} break;
		case PPU_REG_OBP0:
		{

		} break;
		case PPU_REG_OBP1:
		{

		} break;
		case PPU_REG_WY:
		{

		} break;
		case PPU_REG_WX:
		{

		} break;
	}

	return 0x00;
}

void PPU::write(word addr, byte value)
{
	switch (addr)
	{
		case PPU_REG_LCDC:
		{

		} break;
		case PPU_REG_STAT:
		{

		} break;
		case PPU_REG_SCY:
		{

		} break;
		case PPU_REG_SCX:
		{

		} break;
		case PPU_REG_LY:
		{

		} break;
		case PPU_REG_LYC:
		{

		} break;
		case PPU_REG_DMA:
		{

		} break;
		case PPU_REG_BGP:
		{

		} break;
		case PPU_REG_OBP0:
		{

		} break;
		case PPU_REG_OBP1:
		{

		} break;
		case PPU_REG_WY:
		{

		} break;
		case PPU_REG_WX:
		{

		} break;
	}
}

}