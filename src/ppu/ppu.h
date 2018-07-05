#ifndef PPU_H
#define PPU_H

#include "mem/memory_area.h"

namespace hgb
{

#define PPU_REG_S		0xFF40	// ppu memory area start
#define PPU_REG_E		0xFF4B	// ppu memory area end
#define PPU_REG_LCDC	0xFF40	// lcd control (R/W)
#define PPU_REG_STAT	0xFF41	// lcd control status (R/W)
#define PPU_REG_SCY		0xFF42	// scroll y (R/W)
#define PPU_REG_SCX		0xFF43	// scroll x (R/W)
#define PPU_REG_LY		0xFF44	// lcd controller y-coordinate (R)
#define PPU_REG_LYC		0xFF45	// LY compare (R/W)
#define PPU_REG_DMA		0xFF46	// dma transfer and start address (W)
#define PPU_REG_BGP		0xFF47	// bg palette data (R/W)
#define PPU_REG_OBP0	0xFF48	// object palette 0 data (R/W)
#define PPU_REG_OBP1	0xFF49	// object palette 1 data (R/W)
#define PPU_REG_WY		0xFF4A	// window y position (R/W)
#define PPU_REG_WX		0xFF4B	// window x position minus 7 (R/W)

class PPU : public MemoryArea
{
public:
	PPU();
	~PPU();

	// Run the PPU, one cycle at a time
	void tick();

	virtual byte read(word addr) override;
	virtual void write(word addr, byte value) override;
private:
	byte LCDC;
	byte STAT;
	byte SCY;
	byte SCX;
	byte LY;
	byte LYC;
	byte DMA;
	byte BGP;
	byte OBP0;
	byte OBP1;
	byte WY;
	byte WX;
};

}

#endif // PPU_H